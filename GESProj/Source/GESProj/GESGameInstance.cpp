// Fill out your copyright notice in the Description page of Project Settings.


#include "GESGameInstance.h"

#if !WITH_EDITOR
#include "GES.h"
#endif

void UGESGameInstance::Init()
{
#if !WITH_EDITOR
	FGES::Init();
#endif

	Super::Init();
}

void UGESGameInstance::OnStart()
{
	Super::OnStart();
}

void UGESGameInstance::Shutdown()
{
#if !WITH_EDITOR
	FGES::Clear();
#endif
	
	Super::Shutdown();
}
