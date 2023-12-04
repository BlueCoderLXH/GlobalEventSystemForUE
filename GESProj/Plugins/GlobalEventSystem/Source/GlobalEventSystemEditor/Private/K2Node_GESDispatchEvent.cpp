// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_GESDispatchEvent.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "GlobalEventSystemBPLibrary.h"
#include "GraphEditorSettings.h"
#include "KismetCompiler.h"
#include "ToolMenu.h"
#include "Kismet2/BlueprintEditorUtils.h"

using namespace GESEditorConstant;

UK2Node_GESDispatchEvent::UK2Node_GESDispatchEvent()
{
#if WITH_EDITORONLY_DATA
	// Is 'Rename Node' or not
	bCanRenameNode = 0;
#endif
}

#if WITH_EDITOR
TSharedPtr<SGraphNode> UK2Node_GESDispatchEvent::CreateVisualWidget()
{
	return SNew(SGraphNodeK2_GESDispatchEvent, this);
}
#endif

void UK2Node_GESDispatchEvent::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_GESDispatchEvent::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (!Context || Context->bIsDebugging || !Context->Pin) return;

	// The first thee pin(Input_Exec, Output_Exec, EventType) shouldn't handle insert/remove UI action
	const UEdGraphPin* SelectedPin = Context->Pin;
	const int32 SelectPinIndex = GetParamPinIndex(SelectedPin) - DynamicPinIndexStart;
	if (SelectPinIndex <= INDEX_NONE)
	{
		return;
	}
	
	if (!SelectedPin->ParentPin &&
		SelectedPin->Direction == EGPD_Input)
	{
		FToolMenuSection& MenuSection = Menu->AddSection(TEXT("UK2Node_GESDispatchEvent"), FText::FromString(TEXT("GESDispatchEvent")));

		// Insert Before Pin UI Action
		const FUIAction InsertBeforePinUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_GESDispatchEvent*>(this), &UK2Node_GESDispatchEvent::InsertBeforeInputPin, const_cast<UEdGraphPin*>(Context->Pin)));
		const FToolMenuEntry InsertBeforePinMenuEntry = FToolMenuEntry::InitMenuEntry(
			TEXT("UK2Node_GESDispatchEvent_InsertParamBefore"),
			FText::FromString(FString::Printf(TEXT("Insert Before EventParam%d"), SelectPinIndex)),
			FText::FromString(TEXT("Insert a param pin before current")),
			FSlateIcon(),
			InsertBeforePinUIAction);
		MenuSection.AddEntry(InsertBeforePinMenuEntry);
		
		// Insert After Pin UI Action
		const FUIAction InsertAfterPinUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_GESDispatchEvent*>(this), &UK2Node_GESDispatchEvent::InsertAfterInputPin, const_cast<UEdGraphPin*>(Context->Pin)));
		const FToolMenuEntry InsertAfterPinMenuEntry = FToolMenuEntry::InitMenuEntry(
			TEXT("UK2Node_GESDispatchEvent_InsertParamAfter"),
			FText::FromString(FString::Printf(TEXT("Insert After EventParam%d"), SelectPinIndex)),
			FText::FromString(TEXT("Insert a param pin after current")),
			FSlateIcon(),
			InsertAfterPinUIAction);
		MenuSection.AddEntry(InsertAfterPinMenuEntry);
		
		// Remove Pin UI Action
		const FUIAction RemovePinUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_GESDispatchEvent*>(this), &UK2Node_GESDispatchEvent::RemoveInputPin, const_cast<UEdGraphPin*>(Context->Pin)));
		const FToolMenuEntry RemovePinMenuEntry = FToolMenuEntry::InitMenuEntry(
			TEXT("UK2Node_GESDispatchEvent_RemoveParam"),
			FText::FromString(FString::Printf(TEXT("Remove EventParam%d"), SelectPinIndex)),
			FText::FromString(TEXT("Remove current param pin")),
			FSlateIcon(),
			RemovePinUIAction);
		MenuSection.AddEntry(RemovePinMenuEntry);
	}
}

void UK2Node_GESDispatchEvent::AllocateDefaultPins()
{
	// Input Pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	// Output Pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FGESEventStruct::StaticStruct();
	CreatePin(EGPD_Input, PinType, TEXT("GlobalEventStruct"));

	// EventType Pin
	UEdGraphPin* EventTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EventTypePinName);
	if (EventTypePin)
	{
		EventTypePin->PinToolTip = TEXT("Event type defined in 'Enums.lua'");
	}

	// Create default event param pins
	for (int32 Index = DynamicPinIndexStart; Index < DynamicPinIndexStart + ParamPinCount; Index++)
	{
		CreateNextParamPin(Index, true);
	}
	
	Super::AllocateDefaultPins();
}

void UK2Node_GESDispatchEvent::AddInputPin()
{
	Modify();
	
	CreateNextParamPin();
}

void UK2Node_GESDispatchEvent::CreateNextParamPin(const int32 Index, const bool bInitial)
{
	UEdGraphPin* ParamPin;
	if (Index != INDEX_NONE)
	{
		FEdGraphPinType PinType;
		PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		ParamPin = CreatePin(EGPD_Input, PinType, GetUniquePinName(Index - DynamicPinIndexStart), Index);
	}
	else
	{
		ParamPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetUniquePinName(Index));
	}
	
	if (ParamPin)
	{
		ParamPin->PinToolTip = TEXT("Params for this event");

		if (!bInitial)
		{
			ParamPinCount++;
		}
	}
}

FName UK2Node_GESDispatchEvent::GetUniquePinName(const int32 Index) const
{
	FName NewPinName;
	if (Index != INDEX_NONE)
	{
		NewPinName = *GetPinNameByIndex(Index);
		return NewPinName;
	}

	NewPinName = *GetPinNameByIndex(ParamPinCount);
	check(!FindPin(NewPinName));
	return NewPinName;
}

void UK2Node_GESDispatchEvent::InsertBeforeInputPin(UEdGraphPin* Pin)
{
	if (!Pin) return;

	const int32 CurrentPinIndex = GetParamPinIndex(Pin);

	Modify();
	
	CreateNextParamPin(CurrentPinIndex);

	FlushInputPinNames(CurrentPinIndex + 1, Pins.Num());
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());	
}

void UK2Node_GESDispatchEvent::InsertAfterInputPin(UEdGraphPin* Pin)
{
	if (!Pin) return;

	const int32 CurrentPinIndex = GetParamPinIndex(Pin);

	Modify();
	
	CreateNextParamPin(CurrentPinIndex + 1);

	FlushInputPinNames(CurrentPinIndex + 2, Pins.Num());
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());	
}

void UK2Node_GESDispatchEvent::RemoveInputPin(UEdGraphPin* Pin)
{
	if (!Pin) return;
	
	FScopedTransaction Transaction(FText::FromString("DispatchEvent_RemoveInputPin"));
	Modify();
	
	ParamPinCount--;
	
	const int32 RemovePinIndex = GetParamPinIndex(Pin);
	RemovePin(Pin);
	
	FlushInputPinNames(RemovePinIndex, Pins.Num());

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

int32 UK2Node_GESDispatchEvent::GetParamPinIndex(const UEdGraphPin* Pin) const
{
	if (!Pin) return INDEX_NONE; 
	
	for (int32 PinIndex = DynamicPinIndexStart; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex] == Pin)
		{
			return PinIndex;
		}
	}

	return INDEX_NONE;
}

void UK2Node_GESDispatchEvent::FlushInputPinNames(const int32 StartIndex, const int32 EndIndex)
{
	for (int32 PinIndex = StartIndex; PinIndex < EndIndex; PinIndex++)
	{
		Pins[PinIndex]->PinName = *GetPinNameByIndex(PinIndex - DynamicPinIndexStart);
	}
}

void UK2Node_GESDispatchEvent::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* ThenPin = GetThenPin();
	if (ExecPin && ThenPin)
	{
		// Create a CallFunction node for function 'GESDispatchEvent'
		const FName DispatchEventFuncName = GET_FUNCTION_NAME_CHECKED(UGlobalEventSystemBPLibrary, GESDispatchEvent);
		UK2Node_CallFunction* DispatchEventFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		DispatchEventFuncNode->FunctionReference.SetExternalMember(DispatchEventFuncName, UGlobalEventSystemBPLibrary::StaticClass());
		DispatchEventFuncNode->AllocateDefaultPins();

		// Move exec pins
		CompilerContext.MovePinLinksToIntermediate(*ExecPin, *(DispatchEventFuncNode->GetExecPin()));
		CompilerContext.MovePinLinksToIntermediate(*ThenPin, *(DispatchEventFuncNode->GetThenPin()));

		// Move 'EventType' pin
		UEdGraphPin* SourceEventTypePin = FindPinChecked(EventTypePinName);
		UEdGraphPin* TargetEventTypePin = DispatchEventFuncNode->FindPinChecked(EventTypePinName);
		CompilerContext.MovePinLinksToIntermediate(*SourceEventTypePin, *TargetEventTypePin);

		// Move param pins
		for (int32 ParamPinIndex = 0; ParamPinIndex < ParamPinCount; ParamPinIndex++)
		{
			const FName ParamPinName = *GetPinNameByIndex(ParamPinIndex);

			UEdGraphPin* SourceParamPin = FindPinChecked(ParamPinName);

			const FEdGraphPinType& PinType = SourceParamPin->LinkedTo[0]->PinType;
			// Create dynamic param pins for GESDispatchEventFuncNode
			UEdGraphPin* TargetParamPin = DispatchEventFuncNode->CreatePin(EGPD_Input, PinType, ParamPinName, ParamPinIndex + DynamicPinIndexStart);
			check(TargetParamPin);

			CompilerContext.MovePinLinksToIntermediate(*SourceParamPin, *TargetParamPin);
		}
	}

	// Break any links to the expanded node
	BreakAllNodeLinks();	
}


void SGraphNodeK2_GESDispatchEvent::Construct(const FArguments& InArgs, UK2Node* InNode)
{
	ensure(InNode && InNode->GetClass()->ImplementsInterface(UK2Node_AddPinInterface::StaticClass()));
	GraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);

	UpdateGraphNode();
}

void SGraphNodeK2_GESDispatchEvent::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	const TSharedRef<SWidget> AddPinButton = AddPinButtonContent(
		NSLOCTEXT("GESDispatchEvent", "GESDispatchEventAddPinButton", "Add Param"),
		NSLOCTEXT("GESDispatchEvent", "GESDispatchEventAddPinButton_ToolTip", "Add New Param"));

	FMargin AddPinPadding = Settings->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot()
	.AutoHeight()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Right)
	.Padding(AddPinPadding)
	[
		AddPinButton
	];
}

EVisibility SGraphNodeK2_GESDispatchEvent::IsAddPinButtonVisible() const
{
	const IK2Node_AddPinInterface* AddPinNode = Cast<IK2Node_AddPinInterface>(GraphNode);
	ensure(AddPinNode);
	return ((AddPinNode && AddPinNode->CanAddPin()) ? EVisibility::Visible : EVisibility::Collapsed);	
}

FReply SGraphNodeK2_GESDispatchEvent::OnAddPin()
{
	IK2Node_AddPinInterface* AddPinNode = Cast<IK2Node_AddPinInterface>(GraphNode);
	ensure(AddPinNode);
	if (AddPinNode && AddPinNode->CanAddPin())
	{
		FScopedTransaction Transaction(NSLOCTEXT("GESDispatchEvent", "AddPinTransaction", "Add Pin"));

		AddPinNode->AddInputPin();
		UpdateGraphNode();
		GraphNode->GetGraph()->NotifyGraphChanged();
	}
	
	return FReply::Handled();
}
