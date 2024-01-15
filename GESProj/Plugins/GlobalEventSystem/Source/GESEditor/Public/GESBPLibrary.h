﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GES.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GESBPLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FGESDelegateForBP, UPARAM(ref) FGESEventDataArray&, EventDataArray);

/**
 * UGESBPLibrary
 * Implement GES bp utility function
 */
UCLASS()
class GESEDITOR_API UGESBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * GESDispatchEvent
	 * Dispatch a global event from blueprint to native 
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="GlobalEventSystem", meta=(Variadic, BlueprintInternalUseOnly="true"))
	static void GESDispatchEvent(FName EventType);
	DECLARE_FUNCTION(execGESDispatchEvent);
	static void GESDispatchEventInternal(FProperty* PropertyPtr, const void* DataPtr, const FGESEventDataType& Type, FGESEventDataArray& EventDataArray);

	/*
	 * GESRegisterEvent
	 * Register a global event from blueprint
	 */
	UFUNCTION(BlueprintCallable, Category="GlobalEventSystem", meta=(BlueprintInternalUseOnly="true"))
	static void GESRegisterEvent(FName EventType, FGESDelegateForBP EventDelegate);

	/*
	 * GESUnregisterEvent
	 * Unregister a global event from blueprint
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="GlobalEventSystem", meta=(ToolTip="Unregister event for current blueprint", Keywords="GES,UE,UGE"))
	static void GESUnregisterEvent(FName EventType);
	DECLARE_FUNCTION(execGESUnregisterEvent);

	/*
	 * GESConvertEventData
	 * Convert event data array to every single type-specified data
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="GlobalEventSystem", meta=(Variadic, BlueprintInternalUseOnly="true"))
	static void GESConvertEventData(UPARAM(ref) FGESEventDataArray& EventDataArray);
	DECLARE_FUNCTION(execGESConvertEventData);

	/*
	 * DispatchEventLua
	 * Dispatch event from lua
	 */
	UFUNCTION(BlueprintCallable, Category="GES_Lua", meta=(BlueprintInternalUseOnly="true"))
	static void DispatchEventLua(UPARAM(ref) const FGESEventDataArray& EventData);

	/*
	 * RegisterEventLua
	 * Register event from lua
	 */	
	UFUNCTION(BlueprintCallable, Category="GES_Lua", meta=(BlueprintInternalUseOnly="true"))
	static void RegisterEventLua(const FName EventType, FGESDelegateForBP EventDelegate);

	/*
	 * UnregisterEventLua
	 * Unregister event from lua
	 */		
	UFUNCTION(BlueprintCallable, Category="GES_Lua", meta=(BlueprintInternalUseOnly="true"))
	static void UnregisterEventLua(const FName EventType, FGESDelegateForBP EventDelegate);
};