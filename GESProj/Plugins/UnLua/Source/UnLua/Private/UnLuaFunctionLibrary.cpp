﻿// Tencent is pleased to support the open source community by making UnLua available.
// 
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and limitations under the License.

#include "UnLua.h"
#include "UnLuaFunctionLibrary.h"
#include "UnLuaDelegates.h"
#include "UnLuaModule.h"

FString UUnLuaFunctionLibrary::GetScriptRootPath()
{
    return FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + TEXT("Script/"));
}

int64 UUnLuaFunctionLibrary::GetFileLastModifiedTimestamp(FString Path)
{
    const FDateTime FileTime = IFileManager::Get().GetTimeStamp(*Path);
    return FileTime.GetTicks();
}

void UUnLuaFunctionLibrary::HotReload()
{
    IUnLuaModule::Get().HotReload();
}

void UUnLuaFunctionLibrary::OpenPreGarbageCollect(bool bOpen){
    UnLua::FLuaEnv::GOpenPreGarbageCollect = bOpen;
}

void UUnLuaFunctionLibrary::SetLuaGCType(int32 InType){
    UnLua::FLuaEnv::GLuaGCType = InType;
}

int32 UUnLuaFunctionLibrary::GetLuaGCType(){
    return UnLua::FLuaEnv::GLuaGCType;
}
