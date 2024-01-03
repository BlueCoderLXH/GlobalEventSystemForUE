// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GESEventConfig.generated.h"

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
	Array,
	Set,
	Map,
};

USTRUCT(BlueprintType)
struct GLOBALEVENTSYSTEM_API FGESEventDataType
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

	// Only if it is a TMap, 'CppType' is for the key, 'ValueType' is for the value; Otherwise, ValueType is meaningless
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
			UEnum* ScriptEnumPtr = FindObject<UEnum>(ANY_PACKAGE, *InSubTypeName.ToString(), true);
			check(ScriptEnumPtr);
			return ScriptEnumPtr;
		}
		
		return nullptr;
	}
};

USTRUCT(BlueprintType)
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