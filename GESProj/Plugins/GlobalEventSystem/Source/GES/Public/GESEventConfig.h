// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaBase.h"
#include "UnLuaLegacy.h"
#include "UObject/Object.h"
#include "GESEventConfig.generated.h"

extern GES_API TAutoConsoleVariable<bool> CVarGESHotReloadConfig;

UENUM(BlueprintType)
enum class EGESCppType : uint8
{
	None = 0,
	Any UMETA(Hidden),
	Null UMETA(Hidden),
	Bool,
	Integer,
	Float,
	FString,
	FName,
	FText,
	UEnum,
	UStruct,
	UObject,
	UClass,
};

UENUM(BlueprintType)
enum class EGESContainerType : uint8
{
	None = 0,
	TArray,
	TSet,
	TMap,
};

USTRUCT(BlueprintType)
struct GES_API FGESEventDataType
{
	GENERATED_BODY()

	// ContainerType, TArray/TSet/TMap 
	UPROPERTY(EditAnywhere)
	EGESContainerType ContainerType = EGESContainerType::None;

	// CppType
	UPROPERTY(EditAnywhere)
	EGESCppType CppType = EGESCppType::None;

	// CppType's sub type for UEnum or UStruct
	UPROPERTY(EditAnywhere)
	FName CppSubTypeName = NAME_None;

	// Only if it is a TMap<key, value>, 'CppType' is for the key, 'ValueType' is for the value; Otherwise, ValueType is meaningless should be 'None'
	UPROPERTY(EditAnywhere)
	EGESCppType ValueType = EGESCppType::None;

	// ValueType's sub type for UEnum or UStruct
	UPROPERTY(EditAnywhere)
	FName ValueSubTypeName = NAME_None;	
	
	FGESEventDataType() {}
	FGESEventDataType(
		const EGESCppType InCppType,
		const EGESContainerType InContainerType = EGESContainerType::None,
		const FName& InSubTypeName = NAME_None)
	{
		CppType = InCppType;
		ContainerType = InContainerType;
		CppSubTypeName = InSubTypeName;
	}

	FName GetTypeName() const
	{
		return GetTypeNameInner(CppType);
	}

	UObject* GetSubTypeObject() const
	{
		return GetSubTypeObjectInner(CppType, CppSubTypeName);
	}

	FName GetValueTypeName() const
	{
		return GetTypeNameInner(ValueType);
	}

	UObject* GetValueSubTypeObject() const
	{
		return GetSubTypeObjectInner(ValueType, ValueSubTypeName);
	}	

	EPinContainerType GetPinContainerType() const
	{
		return static_cast<EPinContainerType>((static_cast<int32>(ContainerType)));
	}

private:
	static FName GetTypeNameInner(const EGESCppType InCppType)
	{
		switch (InCppType)
		{
		case EGESCppType::None:
			return TEXT("");
			
		case EGESCppType::Any:
		case EGESCppType::Null:
			check(0);
			
		case EGESCppType::Bool:
			return TEXT("bool");
		case EGESCppType::Integer:
			return TEXT("int");
		case EGESCppType::Float:
			return TEXT("float");
		case EGESCppType::FString:
			return TEXT("string");
		case EGESCppType::FName:
			return TEXT("name");
		case EGESCppType::FText:
			return TEXT("text");
		case EGESCppType::UEnum:
			return TEXT("byte");
		case EGESCppType::UStruct:
			return TEXT("struct");
		case EGESCppType::UObject:
			return TEXT("object");
		case EGESCppType::UClass:
			return TEXT("class");
		}
		return NAME_None;
	}

	static UObject* GetSubTypeObjectInner(const EGESCppType InCppType, const FName InSubTypeName)
	{
		if (InCppType == EGESCppType::UStruct && !InSubTypeName.IsNone())
		{
			UScriptStruct* ScriptStructPtr = FindObject<UScriptStruct>(ANY_PACKAGE, *InSubTypeName.ToString());
			check(ScriptStructPtr);
			return ScriptStructPtr;
		}

		if (InCppType == EGESCppType::UEnum && !InSubTypeName.IsNone())
		{
			UEnum* ScriptEnumPtr = FindObject<UEnum>(ANY_PACKAGE, *InSubTypeName.ToString());
			check(ScriptEnumPtr);
			return ScriptEnumPtr;
		}

		if ((InCppType == EGESCppType::UObject || InCppType == EGESCppType::UClass) && !InSubTypeName.IsNone())
		{
			UClass* ScriptClassPtr = FindObject<UClass>(ANY_PACKAGE, *InSubTypeName.ToString());
			check(ScriptClassPtr);
			return ScriptClassPtr;
		}
		
		return nullptr;
	}
};

USTRUCT(BlueprintType)
struct GES_API FGESEventConfigItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName EventType;

	UPROPERTY(EditAnywhere)
	TArray<FGESEventDataType> EventDataTypes;

	bool IsValid() const { return !EventType.IsEqual(NAME_None); }
	
	int32 GetEventDataNum() const { return EventDataTypes.Num(); }
};

USTRUCT(BlueprintType)
struct GES_API FGESEventConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FGESEventConfigItem> Events;

	bool IsValid() const { return Events.Num() > 0; }
	void Clear() { Events.Empty(); }
};

struct GES_API FGESEventConfigHelper
{
	static bool Init()
	{
		const FGESEventConfig& EventConfig = Get();
		return EventConfig.IsValid();
	}

	static void Clear()
	{
		Get().Clear();
	}

	static bool GetConfig(TArray<FGESEventConfigItem>& OutEvents)
	{
		const FGESEventConfig& EventConfig = Get();
		if (!EventConfig.IsValid())
		{
			return false;
		}

		OutEvents = EventConfig.Events;
		return true;
	}

	static bool FindEvent(const FName& EventName, FGESEventConfigItem& OutEventConfig)
	{
		if (IsRunningCommandlet())
		{
			return true;
		}

		TArray<FGESEventConfigItem> Events;
		if (!GetConfig(Events))
		{
			return false;
		}
		
		const FGESEventConfigItem* FindEventConfigItemPtr = Events.FindByPredicate([&](const FGESEventConfigItem& EventConfigItem)
		{
			return EventConfigItem.EventType == EventName;
		});

		if (FindEventConfigItemPtr)
		{
			OutEventConfig = *FindEventConfigItemPtr;
		}

		return !!FindEventConfigItemPtr;
	}

	static bool FindEvent(const FName& EventName)
	{
		FGESEventConfigItem OutEventConfig;
		return FindEvent(EventName, OutEventConfig);
	}

private:
	static FGESEventConfig& Get()
	{
		static FGESEventConfig EventConfig;

		// if with editor, hot reload cpp events config to make it more convenient
		if (!EventConfig.IsValid() || CVarGESHotReloadConfig.GetValueOnGameThread())
		{
			lua_State* L = UnLua::GetState();
			check(L);

			if (CVarGESHotReloadConfig.GetValueOnGameThread())
			{
				UnLua::RunFile(L, TEXT("GlobalEventSystem/CppEvents.lua"));
			}

			const auto LuaRetResult = UnLua::Call(UnLua::GetState(), "Global_GetCppEvents");
			if (LuaRetResult.Num() > 0)
			{
				EventConfig = LuaRetResult[0].Value<FGESEventConfig>();
			}
		}
		
		return EventConfig;
	}
};