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
	const FName OnEventPinName = TEXT("OnEvent");
	const FString EventDataPinPrefixName = TEXT("EventData");
	
}