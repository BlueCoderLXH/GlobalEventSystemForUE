// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GESEventConfig.generated.h"

UENUM(BlueprintType)
enum class EGESEventDataCppType : uint8
{
	EDT_None = 0,
	EDT_Any UMETA(Hidden),
	EDT_Null UMETA(Hidden),
	EDT_Bool,
	EDT_Integer,
	EDT_Float,
	EDT_String,
	EDT_Name,
	EDT_Text,
	EDT_Enum,
	EDT_Struct,
	EDT_Object,
	EDT_SoftObject,
	EDT_Class,
	EDT_SoftClass,
};

UENUM(BlueprintType)
enum class EGESContainerType : uint8
{
	EDCT_None = 0,
	EDCT_Array,
	EDCT_Map,
	EDCT_Set
};

USTRUCT(Blueprintable, BlueprintType)
struct GLOBALEVENTSYSTEM_API FGESEventDataType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EGESEventDataCppType CppType = EGESEventDataCppType::EDT_None;
	
	UPROPERTY(EditAnywhere)
	FName SubTypeName = NAME_None;

	UPROPERTY(EditAnywhere)
	EGESContainerType ContainerType = EGESContainerType::EDCT_None;

	FGESEventDataType() {}
	FGESEventDataType(
		const EGESEventDataCppType InCppType,
		const EGESContainerType InContainerType = EGESContainerType::EDCT_None,
		const FName& InSubTypeName = NAME_None)
	{
		CppType = InCppType;
		ContainerType = InContainerType;
		SubTypeName = InSubTypeName;
	}

	FName GetTypeName() const
	{
		switch (CppType)
		{
		case EGESEventDataCppType::EDT_None:
		case EGESEventDataCppType::EDT_Any:
		case EGESEventDataCppType::EDT_Null:
			check(0);
			
		case EGESEventDataCppType::EDT_Bool:
			return TEXT("bool");
		case EGESEventDataCppType::EDT_Integer:
			return TEXT("int");
		case EGESEventDataCppType::EDT_Float:
			return TEXT("float");
		case EGESEventDataCppType::EDT_String:
			return TEXT("string");
		case EGESEventDataCppType::EDT_Name:
			return TEXT("name");
		case EGESEventDataCppType::EDT_Text:
			return TEXT("text");
		case EGESEventDataCppType::EDT_Enum:
			return TEXT("byte");
		case EGESEventDataCppType::EDT_Struct:
			return TEXT("struct");
		case EGESEventDataCppType::EDT_Object:
			return TEXT("object");
		case EGESEventDataCppType::EDT_SoftObject:
			return TEXT("softobject");
		case EGESEventDataCppType::EDT_Class:
			return TEXT("class");
		case EGESEventDataCppType::EDT_SoftClass:
			return TEXT("softclass");
		}
		return NAME_None;
	}

	UObject* GetSubTypeObject() const
	{
		if (CppType == EGESEventDataCppType::EDT_Struct && !SubTypeName.IsNone())
		{
			UScriptStruct* ScriptStructPtr = FindObject<UScriptStruct>(ANY_PACKAGE, *SubTypeName.ToString());
			check(ScriptStructPtr);
			return ScriptStructPtr;
		}

		if (CppType == EGESEventDataCppType::EDT_Enum && !SubTypeName.IsNone())
		{
			UEnum* ScriptEnumPtr = FindObject<UEnum>(ANY_PACKAGE, *SubTypeName.ToString(), true);
			check(ScriptEnumPtr);
			return ScriptEnumPtr;
		}
		
		return nullptr;
	}

	EPinContainerType GetPinContainerType() const
	{
		return static_cast<EPinContainerType>((static_cast<int32>(ContainerType)));
	}
};

USTRUCT(Blueprintable, BlueprintType)
struct GLOBALEVENTSYSTEM_API FGESEventConfigItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName EventType;

	UPROPERTY(EditAnywhere)
	TArray<FGESEventDataType> EventDataTypes;

	bool IsValid() const { return !EventType.IsEqual(NAME_None); }
	
	int32 GetEventDataNum() const { return EventDataTypes.Num(); }
};

UCLASS(Blueprintable, BlueprintType)
class GLOBALEVENTSYSTEM_API UGESEventConfig : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FGESEventConfigItem> Events;
};

UCLASS(DefaultConfig, Config=GlobalEventSystem)
class GLOBALEVENTSYSTEM_API UGESEventConfigHelper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	FString EventConfigClassPath;

	static const UGESEventConfig* Init()
	{
		const UGESEventConfigHelper* EventConfigHelper = Cast<UGESEventConfigHelper>(StaticClass()->GetDefaultObject());
		if (!EventConfigHelper)
		{
			return nullptr;
		}

		const FSoftClassPath EventConfigClassPath(EventConfigHelper->EventConfigClassPath);
		const UClass* EventConfigClass = EventConfigClassPath.ResolveClass();
		if (!EventConfigClass)
		{
			EventConfigClass = EventConfigClassPath.TryLoadClass<UGESEventConfig>();
		}
		
		if (!IsValid(EventConfigClass))
		{
			return nullptr;
		}
		
		const UGESEventConfig* EventConfig = Cast<UGESEventConfig>(EventConfigClass->GetDefaultObject());
		return EventConfig;
	}

	static bool GetConfig(TArray<FGESEventConfigItem>& OutEvents)
	{
		const UGESEventConfig* EventConfig = Init();
		if (!IsValid(EventConfig))
		{
			return false;
		}

		OutEvents = EventConfig->Events;
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

		// check(FindEventConfigItemPtr);

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
};