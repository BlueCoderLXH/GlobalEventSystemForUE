// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_GESDispatchEvent.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "GESBPLibrary.h"
#include "GraphEditorSettings.h"
#include "K2Node_AssignmentStatement.h"
#include "KismetCompiler.h"
#include "ToolMenu.h"
#include "GESEditorCommon.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"

using namespace GESEditorConstant;

#define LOCTEXT_NAMESPACE "UK2Node_GESDispatchEvent"

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
		}
	}

	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "Internal connection error. @@").ToString(), this);
	}

	// Break any links to the expanded node
	BreakAllNodeLinks();	
}