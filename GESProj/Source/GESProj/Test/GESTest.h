// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GESTest.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GESPROJ_API AGESTest : public AActor
{
	GENERATED_BODY()

public:
	AGESTest();

	UPROPERTY(EditAnywhere)
	FString TestStr;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
