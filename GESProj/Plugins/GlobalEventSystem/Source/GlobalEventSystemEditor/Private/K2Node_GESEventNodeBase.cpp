// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\K2Node_GESEventNodeBase.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "GESEditorCommon.h"
#include "GESEventConfig.h"
#include "Kismet2/BlueprintEditorUtils.h"

using namespace GESEditorConstant;

void UK2Node_GESEventNodeBase::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
		check(NodeSpawner);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}	
}

void UK2Node_GESEventNodeBase::AllocateDefaultPins()
{
	// Input Pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	// Output Pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// EventType Pin
	UEdGraphPin* EventTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EventTypePinName);
	if (EventTypePin)
	{
		EventTypePin->PinToolTip = TEXT("Event type defined in 'Enums.lua'");
		if (!EventTypeValue.IsNone())
		{
			EventTypePin->DefaultValue = EventTypeValue.ToString();
		}
	}

	BeforeAllocateEventDataPins();
	// Create default event param pins
	AllocateEventDataPins();
	AfterAllocateEventDataPins();
}

void UK2Node_GESEventNodeBase::AllocateEventDataPins()
{
	if (EventTypeValue.IsNone() || !EventDataPinInfos.Num()) return;
	
	for (int32 Index = 0; Index < EventDataPinInfos.Num(); Index++)
	{
		const FEdGraphPinType& EventPinType = EventDataPinInfos[Index].PinType;
		UEdGraphPin* EventDataPin = CreatePin(EventDataPinDirection, EventPinType, EventDataPinInfos[Index].PinName);

		if (EventPinType.PinCategory == UEdGraphSchema_K2::PC_Byte || EventPinType.PinCategory == UEdGraphSchema_K2::PC_Enum)
		{
			const UEnum* EnumPtr = Cast<UEnum>(EventPinType.PinSubCategoryObject);
			check(EnumPtr);
			EventDataPin->DefaultValue = EnumPtr->GetNameByIndex(0).ToString();
		}
	}
}

void UK2Node_GESEventNodeBase::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (!Pin) return;

	if (Pin->PinName == EventTypePinName)
	{
		OnEventTypePinValueChanged(Pin);
	}
}

bool UK2Node_GESEventNodeBase::OnEventTypePinValueChanged(UEdGraphPin* Pin)
{
	const FName NewEventTypeValue = *Pin->GetDefaultAsString();
	if (NewEventTypeValue == EventTypeValue) return false;
	
	EventTypeValue = NewEventTypeValue;

	Modify();

	// Remove old event data pins
	for (int32 Index = 0; Index < EventDataPinInfos.Num(); Index++)
	{
		RemovePin(FindPin(EventDataPinInfos[Index].PinName));
	}
	EventDataPinInfos.Reset();

	FGESEventConfigItem EventItem;
	if (UGESEventConfigHelper::FindEvent(EventTypeValue, EventItem))
	{
		const TArray<FGESEventDataType>& EventDataTypes = EventItem.EventDataTypes;
		// Create new event data pins
		for (int32 Index = 0; Index < EventDataTypes.Num(); Index++)
		{
			FEdGraphPinType PinType;
			PinType.PinCategory = EventDataTypes[Index].GetTypeName();
			PinType.PinSubCategoryObject = EventDataTypes[Index].GetSubTypeObject();
			PinType.ContainerType = EventDataTypes[Index].GetPinContainerType();
			EventDataPinInfos.Add({GetEventDataPinName(Index), PinType});
		}
		AllocateEventDataPins();

		check(EventDataPinInfos.Num() == EventDataTypes.Num());
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());	
	
	return true;
}
