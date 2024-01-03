#pragma once

#include "CoreMinimal.h"
#include "GESEditorCommon.generated.h"

USTRUCT()
struct FGESEventDataPinInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FEdGraphPinType PinType;
};

namespace GESEditorConstant
{
	const FName EventTypePinName = TEXT("EventType");
	const FName EventDataArrayPinName = TEXT("EventDataArray");
	const FName EventDelegatePinName = TEXT("EventDelegate");
	const FName EventDelegateFuncName = TEXT("OnGESEvent");
	const FName OnEventPinName = TEXT("OnEvent");
	const FString EventDataPinPrefixName = TEXT("EventData");

	static FName GetEventDataPinName(const int32 Index)
	{
		return *FString::Printf(TEXT("%s%d"), *EventDataPinPrefixName, Index);
	}

	static FName GetCustomEventFuncName(const FName& EventType)
	{
		return *FString::Printf(TEXT("%s_%s"), *EventDelegateFuncName.ToString(), *EventType.ToString());
	}
}