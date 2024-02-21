// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GES.h"
#include "GameFramework/Actor.h"
#include "GESTest.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogGESTest, Log, All);

UENUM(BlueprintType)
enum EDoTestDispatchType
{
	Cpp,
	Blueprint,
	Lua
};

UCLASS(BlueprintType, Blueprintable)
class GESPROJ_API AGESTest : public AActor
{
	GENERATED_BODY()

public:
	AGESTest();

	UPROPERTY(EditAnywhere)
	bool bDoCppTest_Register;
	
	UPROPERTY(EditAnywhere)
	bool bDoBPTest_Register;

	UPROPERTY(EditAnywhere)
	bool bDoLuaTest_Register;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDoTestDispatchType> DoTestDispatchType;	

	UPROPERTY(EditAnywhere)
	FString TestStr;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DoCppTest_Register();
	void DoCppTest_Unregister();
	void DoCppTest_Dispatch();

	UFUNCTION(BlueprintImplementableEvent)
	void DoBPTest_Register();
	UFUNCTION(BlueprintImplementableEvent)
	void DoBPTest_Unregister();
	UFUNCTION(BlueprintImplementableEvent)
	void DoBPTest_Dispatch();

	UFUNCTION(BlueprintImplementableEvent)
	void DoLuaTest_Register();
	UFUNCTION(BlueprintImplementableEvent)
	void DoLuaTest_Unregister();
	UFUNCTION(BlueprintImplementableEvent)
	void DoLuaTest_Dispatch();	
	
private:
	void OnBoolEvent(const FGESEventDataArray& EventData);
	void OnIntegerEvent(const FGESEventDataArray& EventData);
	void OnFloatEvent(const FGESEventDataArray& EventData);
	void OnStringEvent(const FGESEventDataArray& EventData);
	void OnNameEvent(const FGESEventDataArray& EventData);
	void OnTextEvent(const FGESEventDataArray& EventData);
	void OnEnumEvent(const FGESEventDataArray& EventData);
	void OnStructEvent(const FGESEventDataArray& EventData);
	void OnObjectEvent(const FGESEventDataArray& EventData);
	void OnClassEvent(const FGESEventDataArray& EventData);
	void OnArrayEvent(const FGESEventDataArray& EventData);
	void OnSetEvent(const FGESEventDataArray& EventData);
	void OnMapEvent(const FGESEventDataArray& EventData);

public:
	virtual void Tick(float DeltaTime) override;
};
