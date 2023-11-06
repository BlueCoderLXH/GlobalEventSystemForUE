// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlobalEventSystemModule.h"

#define LOCTEXT_NAMESPACE "GlobalEventSystem"

void FGlobalEventSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGlobalEventSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGlobalEventSystemModule, GlobalEventSystem)