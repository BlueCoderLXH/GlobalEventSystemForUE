// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_GESDispatchEvent.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "GESBPLibrary.h"
#include "GraphEditorSettings.h"
#include "K2Node_AssignmentStatement.h"
#include "KismetCompiler.h"
#include "ToolMenu.h"
#include "GESEditorCommon.h"
#include "K2Node_EnumLiteral.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeMap.h"
#include "K2Node_MakeSet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"

using namespace GESEditorCommon;

#define LOCTEXT_NAMESPACE "UK2Node_GESDispatchEvent"

bool UK2Node_GESDispatchEvent::HandleEventDataPinDefaultConnection(
	UEdGraphPin* SourcePin,
	UEdGraphPin* TargetPin,
	FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph)
{
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	const FEdGraphPinType& PinType = SourcePin->PinType;
	UEdGraphNode* SourceNode = SourcePin->GetOwningNode();
	
	bool bIsErrorFree = true;

	// Container default connection
	if (PinType.ContainerType != EPinContainerType::None)
	{
		UK2Node_MakeContainer* ContainerNode = nullptr;
		
		if (PinType.ContainerType == EPinContainerType::Array)
		{
			ContainerNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(SourceNode, SourceGraph);
		}
		else if (PinType.ContainerType == EPinContainerType::Map)
		{
			ContainerNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeMap>(SourceNode, SourceGraph);
		}
		else if (PinType.ContainerType == EPinContainerType::Set)
		{
			ContainerNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeSet>(SourceNode, SourceGraph);
		}

		check(ContainerNode);

		ContainerNode->NumInputs = 0;
		ContainerNode->AllocateDefaultPins();

		UEdGraphPin* MakeContainerPin = ContainerNode->GetOutputPin();
		bIsErrorFree &= Schema->TryCreateConnection(TargetPin, MakeContainerPin);
		
		return bIsErrorFree;
	}

	// Supported type's default connection
	if (PinType.PinCategory == UEdGraphSchema_K2::PC_Byte)
	{
		UK2Node_EnumLiteral* EnumLiteralNode = CompilerContext.SpawnIntermediateNode<UK2Node_EnumLiteral>(SourceNode, SourceGraph);
		EnumLiteralNode->Enum = Cast<UEnum>(PinType.PinSubCategoryObject.Get());
		EnumLiteralNode->AllocateDefaultPins();

		UEdGraphPin* EnumInputPin = EnumLiteralNode->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName());
		EnumInputPin->DefaultValue = SourcePin->DefaultValue;

		bIsErrorFree &= Schema->TryCreateConnection(TargetPin, EnumLiteralNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
	}
	else
	{
		FMemberReference FunctionReference;
		FName EventDataConvertFuncName;
		
		if (PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && PinType.PinSubCategoryObject.IsValid())
		{
			const FString SubTypeName = PinType.PinSubCategoryObject->GetName();
			if (SubTypeName == TEXT("Vector"))
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UGESBPLibrary, MakeLiteralVector);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UGESBPLibrary::StaticClass());
			}
			else if (SubTypeName == TEXT("Rotator"))
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UGESBPLibrary, MakeLiteralRotator);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UGESBPLibrary::StaticClass());
			}
		}
		else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Class)
		{
			EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UGESBPLibrary, MakeClass);
			FunctionReference.SetExternalMember(
				EventDataConvertFuncName, UGESBPLibrary::StaticClass());
		}
		else
		{
			if (PinType.PinCategory == UEdGraphSchema_K2::PC_Int)
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, MakeLiteralInt);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UKismetSystemLibrary::StaticClass());
			}
			else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, MakeLiteralBool);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UKismetSystemLibrary::StaticClass());
			}
			else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Float)
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, MakeLiteralFloat);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UKismetSystemLibrary::StaticClass());
			}
			else if (PinType.PinCategory == UEdGraphSchema_K2::PC_String)
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, MakeLiteralString);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UKismetSystemLibrary::StaticClass());
			}
			else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, MakeLiteralName);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UKismetSystemLibrary::StaticClass());
			}
			else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Text)
			{
				EventDataConvertFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, MakeLiteralText);
				FunctionReference.SetExternalMember(
					EventDataConvertFuncName, UKismetSystemLibrary::StaticClass());
			}
		}

		// Convert func handle
		if (!EventDataConvertFuncName.IsNone())
		{
			UK2Node_CallFunction* EventDataConvertNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SourceNode, SourceGraph);
			checkf(EventDataConvertNode, TEXT("UK2Node_GESDispatchEvent::GetEventDataConvertFuncNode Unsupported type:%s"), *PinType.PinCategory.ToString());

			EventDataConvertNode->FunctionReference.SetExternalMember(FunctionReference.GetMemberName(), FunctionReference.GetMemberParentClass());
			EventDataConvertNode->AllocateDefaultPins();

			UEdGraphPin* EventDataConvertFuncValPin = EventDataConvertNode->FindPinChecked(TEXT("Value"));
			UEdGraphPin* EventDataConvertFuncRetPin = EventDataConvertNode->GetReturnValuePin();
			EventDataConvertFuncRetPin->PinType = TargetPin->PinType;

			bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*SourcePin, *EventDataConvertFuncValPin).CanSafeConnect();
			bIsErrorFree &= Schema->TryCreateConnection(TargetPin, EventDataConvertFuncRetPin);
		}
		else
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error: @@ with type '@@' mush connect valid pin! @@").ToString(), SourcePin, *PinType.PinCategory.ToString(), SourceNode);
		}
	}
	
	return bIsErrorFree;
}

void UK2Node_GESDispatchEvent::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(Schema);

	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* ThenPin = GetThenPin();
	check(ExecPin && ThenPin);

	bool bIsErrorFree = true;
	
	if (ExecPin && ThenPin)
	{
		// Create a CallFunction node for function 'GESDispatchEvent'
		const FName DispatchEventFuncName = GET_FUNCTION_NAME_CHECKED(UGESBPLibrary, GESDispatchEvent);
		UK2Node_CallFunction* DispatchEventFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		DispatchEventFuncNode->FunctionReference.SetExternalMember(DispatchEventFuncName, UGESBPLibrary::StaticClass());
		DispatchEventFuncNode->AllocateDefaultPins();

		// Move exec pins
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*ExecPin, *(DispatchEventFuncNode->GetExecPin())).CanSafeConnect();
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*ThenPin, *(DispatchEventFuncNode->GetThenPin())).CanSafeConnect();

		// Move 'EventType' pin
		UEdGraphPin* SourceEventTypePin = FindPinChecked(EventTypePinName);
		UEdGraphPin* TargetEventTypePin = DispatchEventFuncNode->FindPinChecked(EventTypePinName);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*SourceEventTypePin, *TargetEventTypePin).CanSafeConnect();
		
		// Move EventData pins
		for (int32 Index = 0; Index < EventDataPinInfos.Num(); Index++)
		{
			const FName& EventDataPinName = EventDataPinInfos[Index].PinName;
			UEdGraphPin* SourceEventDataPin = FindPinChecked(EventDataPinName);
			UEdGraphPin* TargetEventDataPin = DispatchEventFuncNode->CreatePin(EGPD_Input, EventDataPinInfos[Index].PinType, EventDataPinName);
			
			bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*SourceEventDataPin, *TargetEventDataPin).CanSafeConnect();
			
			if (TargetEventDataPin->LinkedTo.Num() <= 0)
			{
				bIsErrorFree &= HandleEventDataPinDefaultConnection(SourceEventDataPin, TargetEventDataPin, CompilerContext, SourceGraph);
			}
		}
	}

	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error. @@").ToString(), this);
	}

	// Break any links to the expanded node
	BreakAllNodeLinks();	
}