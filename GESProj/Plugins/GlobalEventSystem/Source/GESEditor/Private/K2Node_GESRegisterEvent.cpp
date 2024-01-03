// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GESRegisterEvent.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "GESBPLibrary.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "KismetCompiler.h"
#include "GESEditorCommon.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "UK2Node_GESRegisterEvent"

using namespace GESEditorConstant;

void UK2Node_GESRegisterEvent::BeforeAllocateEventDataPins()
{
	// Callback output pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, OnEventPinName);
}

bool UK2Node_GESRegisterEvent::IsEventRegistered() const
{
	const UEdGraph* EdGraph = this->GetGraph();
	if (!EdGraph) return false;

	TArray<UK2Node_CustomEvent*> EventNodes;
	EdGraph->GetNodesOfClass<>(EventNodes);

	for (const UK2Node_CustomEvent* EventNode : EventNodes)
	{
		// For single blueprint, every event can be only registered once
		if (IsValid(EventNode) && EventNode->CustomFunctionName == GetCustomEventFuncName(EventTypeValue))
		{
			return true;
		}
	}

	return false;
}

void UK2Node_GESRegisterEvent::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(Schema);

	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* ThenPin = GetThenPin();
	check(ExecPin && ThenPin);

	bool bIsErrorFree = true;

	// Create a UK2Node_CallFunction for function 'GESDispatchEvent'
	const FName RegisterEventFuncName = GET_FUNCTION_NAME_CHECKED(UGESBPLibrary, GESRegisterEvent);
	UK2Node_CallFunction* RegisterEventFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	{
		RegisterEventFuncNode->FunctionReference.SetExternalMember(RegisterEventFuncName, UGESBPLibrary::StaticClass());
		RegisterEventFuncNode->AllocateDefaultPins();
	}

	// Connect 'EventType' pin
	{
		UEdGraphPin* EventTypePin = FindPinChecked(EventTypePinName);
		if (IsEventRegistered())
		{
			CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("InternalConnectionError", "Event:{0} has been already registered @@"),
				FText::FromString(EventTypeValue.ToString())).ToString(), this);
			return;
		}

		if (!FGESEventConfigHelper::FindEvent(EventTypeValue))
		{
			CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("InternalConnectionError", "Event:{0} hasn't been configured @@"),
				FText::FromString(EventTypeValue.ToString())).ToString(), this);
			return;
		}
		
		UEdGraphPin* RegisterEventFuncEventTypePin = RegisterEventFuncNode->FindPinChecked(EventTypePinName);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*EventTypePin, *RegisterEventFuncEventTypePin).CanSafeConnect();
	}	
	
	// Connect 'exec/then' pins
	{
		UEdGraphPin* RegisterEventFuncExecPin = RegisterEventFuncNode->GetExecPin();
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*ExecPin, *RegisterEventFuncExecPin).CanSafeConnect();
		UEdGraphPin* RegisterEventFuncThenPin = RegisterEventFuncNode->GetThenPin();
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*ThenPin, *RegisterEventFuncThenPin).CanSafeConnect();
	}	

	// Create a 'CustomEvent' node
	UK2Node_CustomEvent* CustomEventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this);
	CustomEventNode->CustomFunctionName = GetCustomEventFuncName(EventTypeValue);
	CustomEventNode->AllocateDefaultPins();

	// Connect 'Delegate' pin
	{
		UEdGraphPin* CustomEventDelegatePin = CustomEventNode->FindPinChecked(UK2Node_CustomEvent::DelegateOutputName);
		UEdGraphPin* RegisterEventDelegatePin = RegisterEventFuncNode->FindPinChecked(EventDelegatePinName);
		bIsErrorFree &= Schema->TryCreateConnection(RegisterEventDelegatePin, CustomEventDelegatePin);
		CustomEventNode->AutowireNewNode(RegisterEventDelegatePin);
	}

	// Create a UK2Node_CallFunction node for 'GESConvertEventData' & 'EventData' pins
	const FName ConvertEventDataFuncName = GET_FUNCTION_NAME_CHECKED(UGESBPLibrary, GESConvertEventData);
	UK2Node_CallFunction* ConvertEventDataFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	{
		ConvertEventDataFuncNode->FunctionReference.SetExternalMember(ConvertEventDataFuncName, UGESBPLibrary::StaticClass());
		ConvertEventDataFuncNode->AllocateDefaultPins();
	}

	// Connect 'EventDataArray' pin
	{
		UEdGraphPin* EventDataArrayPin = ConvertEventDataFuncNode->FindPinChecked(EventDataArrayPinName);
		UEdGraphPin* CustomEventEventDataArrayPin = CustomEventNode->FindPinChecked(EventDataArrayPinName);
		bIsErrorFree &= Schema->TryCreateConnection(CustomEventEventDataArrayPin, EventDataArrayPin);
	}

	// Connect 'EventData' pins
	{
		for (int32 Index = 0; Index < EventDataPinInfos.Num(); Index++)
		{
			const FName& EventDataPinName = EventDataPinInfos[Index].PinName;
			UEdGraphPin* EventDataPin = FindPinChecked(EventDataPinName);
			UEdGraphPin* ConvertFuncEventDataPin = ConvertEventDataFuncNode->CreatePin(EGPD_Output, EventDataPin->PinType, EventDataPinName);
			bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*EventDataPin, *ConvertFuncEventDataPin).CanSafeConnect();			
		}
	}

	// Connect CustomEventEndPin to ConvertEventDataFuncExecPin
	{
		UEdGraphPin* CustomEventEndPin = CustomEventNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
		UEdGraphPin* ConvertEventDataFuncExecPin = ConvertEventDataFuncNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
		bIsErrorFree &= Schema->TryCreateConnection(CustomEventEndPin, ConvertEventDataFuncExecPin);
	}	

	// Connect ConvertEventDataFuncEndPin to OnEventPin
	{
		UEdGraphPin* ConvertEventDataFuncEndPin = ConvertEventDataFuncNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
		UEdGraphPin* OnEventPin = FindPinChecked(OnEventPinName);
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*OnEventPin, *ConvertEventDataFuncEndPin).CanSafeConnect();
	}
	
	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error. @@").ToString(), this);
	}
	
	BreakAllNodeLinks();
}
