// Fill out your copyright notice in the Description page of Project Settings.


#include "GESGameInstance.h"

#include "UnLuaBase.h"

void UGESGameInstance::Init()
{
	UnLua::Startup();

	lua_State* L = UnLua::GetState();
	UnLua::RunFile(L, TEXT("Preload.lua"));

	Super::Init();
}

void UGESGameInstance::OnStart()
{
	// UnLua::Startup();
	
	Super::OnStart();
}

void UGESGameInstance::Shutdown()
{
	UnLua::Shutdown();
	
	Super::Shutdown();
}
