// Fill out your copyright notice in the Description page of Project Settings.


#include "GESGameInstance.h"

#include "GES.h"
#include "UnLuaBase.h"

void UGESGameInstance::Init()
{
	FGES::Init();

	Super::Init();
}

void UGESGameInstance::OnStart()
{
	Super::OnStart();
}

void UGESGameInstance::Shutdown()
{
	FGES::Clear();
	
	Super::Shutdown();
}
