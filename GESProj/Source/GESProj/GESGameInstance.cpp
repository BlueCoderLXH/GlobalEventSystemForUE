// Fill out your copyright notice in the Description page of Project Settings.


#include "GESGameInstance.h"
#include "GES.h"

void UGESGameInstance::Init()
{
	Super::Init();

	// Initialize GES
	FGES::Init();
}

void UGESGameInstance::Shutdown()
{
	// Shutdown GES
	FGES::Shutdown();
	
	Super::Shutdown();
}
