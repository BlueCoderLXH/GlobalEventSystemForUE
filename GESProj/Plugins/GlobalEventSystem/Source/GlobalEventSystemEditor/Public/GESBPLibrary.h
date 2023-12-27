// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEventSystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GESBPLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FGESDelegateForBP, UPARAM(ref) FGESEventDataArray&, EventDataArray);

/**
 * Only for test, will be removed in the future
 */
USTRUCT(BlueprintType)
struct FGESTestStruct
{
	GENERATED_BODY()

	FGESTestStruct() {}

	FGESTestStruct(int32 InInt, FString InStr) : IntValue(InInt), StrValue(InStr) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StrValue;
};

/**
 * UGlobalEventSystemBPLibrary
 * Define
 */
UCLASS()
class GLOBALEVENTSYSTEMEDITOR_API UGESBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * GESDispatchEvent
	 * Dispatch a global event from blueprint to native 
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="GES|DispatchEvent", meta=(Variadic, BlueprintInternalUseOnly="true"))
	static void GESDispatchEvent(FName EventType);
	DECLARE_FUNCTION(execGESDispatchEvent);
	static void GESDispatchEventInternal(FProperty* PropertyPtr, const void* DataPtr, const FGESEventDataType& Type, FGESEventDataArray& EventDataArray);

	/*
	 * GESRegisterEvent
	 * Register a global event from blueprint
	 */
	UFUNCTION(BlueprintCallable, Category="GES|RegisterEvent", meta=(BlueprintInternalUseOnly="true"))
	static void GESRegisterEvent(FName EventType, FGESDelegateForBP EventDelegate);

	/*
	 * GESUnregisterEvent
	 * Unregister a global event from blueprint
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="GES|UnregisterEvent", meta=(ToolTip="Unregister event for current blueprint", Keywords="GES,UE,UGE"))
	static void GESUnregisterEvent(FName EventType);
	DECLARE_FUNCTION(execGESUnregisterEvent);

	/*
	 * GESConvertEventData
	 * Convert event data array to every single type-specified data
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="GES|GESConvertEventData", meta=(Variadic, BlueprintInternalUseOnly="true"))
	static void GESConvertEventData(UPARAM(ref) FGESEventDataArray& EventDataArray);
	DECLARE_FUNCTION(execGESConvertEventData);

	UFUNCTION(BlueprintCallable, Category="GES|DispatchEventLua")
	static void DispatchEventLua(UPARAM(ref) const FGESEventDataArray& EventData);

	UFUNCTION(BlueprintCallable, Category="GES|RegisterEventLua")
	static void RegisterEventLua(const FName EventType, FGESDelegateForBP EventDelegate);

	UFUNCTION(BlueprintCallable, Category="GES|UnregisterEventLua")
	static void UnregisterEventLua(const FName EventType, FGESDelegateForBP EventDelegate);
};