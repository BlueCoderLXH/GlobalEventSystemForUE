// Copyright Epic Games, Inc. All Rights Reserved.

#include "GESModule.h"

#include "GES.h"

#define LOCTEXT_NAMESPACE "GlobalEventSystem"

void FGESModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	FGES::Init();
#endif
}

void FGESModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if WITH_EDITOR
	FGES::Shutdown();
#endif	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGESModule, GES)