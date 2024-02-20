#pragma once

#include "CoreMinimal.h"
#include "GESEventConfig.h"
#include "GESEventData.generated.h"

typedef FName FGESEventType;

/**
 * FGESEventData
 *
 * Wrap the event parameter with supported type
 */
USTRUCT(BlueprintType)
struct GES_API FGESEventData
{
	GENERATED_BODY()
	
	FGESEventData() : Value(FMagicUnion::InValidValue) { }

	FGESEventData(const FGESEventDataType& InType)
	{
		Set(InType, nullptr);
	}
	
	FGESEventData(const FGESEventDataType& InType, void* InValuePtr)
	{
		Set(InType, InValuePtr);
	}

	FGESEventData(const FGESEventDataType& InType, const void* InValuePtr)
	{
		Set(InType, InValuePtr);
	}

	void Set(const FGESEventDataType& InType, const void* InValuePtr)
	{
		Type = InType;
		Value = FMagicUnion::ConvertToInt64(const_cast<void*>(InValuePtr));
	}
	
	const EGESCppType& GetCppType() const { return Type.CppType; }
	
	const EGESContainerType& GetContainerType() const { return Type.ContainerType; }

	void* GetValueRawPtr() const
	{
		return FMagicUnion::ConvertToRawAddr(Value);
	}

private:
	UPROPERTY()
	FGESEventDataType Type;

	UPROPERTY()
	uint64 Value;
	
	union FMagicUnion
	{
		uint64 Value;
		void* Addr;

		template<typename T>
		static T& ConvertToAddr(const uint64& InIntValue)
		{
			check(InIntValue != InValidValue);
			if (InIntValue == NullValue) return *static_cast<T*>(nullptr);
			
			FMagicUnion MagicUnion;
			MagicUnion.Value = InIntValue;
			return *static_cast<T*>(MagicUnion.Addr);
		}

		static void* ConvertToRawAddr(const uint64& InIntValue)
		{
			check(InIntValue != InValidValue);
			if (InIntValue == NullValue) return nullptr;
			
			FMagicUnion MagicUnion;
			MagicUnion.Value = InIntValue;
			return MagicUnion.Addr;
		}

		static uint64 ConvertToInt64(void* InAddr)
		{
			if (!InAddr) return NullValue;
			
			FMagicUnion MagicUnion;
			MagicUnion.Addr = InAddr;
			return MagicUnion.Value;
		}

		static constexpr uint64 InValidValue = -1;
		static constexpr uint64 NullValue = 0;
	};

public:
	void* GetNull() const
	{
		checkf(GetCppType() == EGESCppType::Null && !Value, TEXT("FGESEventDataWrapper:GetNull Type:%s isn't a nullptr"), *GetTypeString());
		return GetValueRawPtr();		
	}
	
	bool GetBool() const
	{
		checkf(GetCppType() == EGESCppType::Bool, TEXT("FGESEventDataWrapper:GetBool Type:%s isn't a bool"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<bool>(Value);
	}

	int8 GetByte() const
	{
		checkf(GetCppType() == EGESCppType::Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int8>(Value);
	}

	int16 GetShort() const
	{
		checkf(GetCppType() == EGESCppType::Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int16>(Value);;		
	}

	int32 GetInt() const
	{
		checkf(GetCppType() == EGESCppType::Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int32>(Value);
	}

	int64 GetLong() const
	{
		checkf(GetCppType() == EGESCppType::Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int64>(Value);
	}

	float GetFloat() const
	{
		checkf(GetCppType() == EGESCppType::Float, TEXT("FGESEventDataWrapper:GetFloat Type:%s isn't a float"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<float>(Value);
	}

	double GetDouble() const
	{
		checkf(GetCppType() == EGESCppType::Float, TEXT("FGESEventDataWrapper:GetDouble Type:%s isn't a float"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<double>(Value);
	}

	const FString& GetString() const
	{
		checkf(GetCppType() == EGESCppType::FString, TEXT("FGESEventDataWrapper:GetString Type:%s isn't a FString"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FString>(Value);
	}

	const FName& GetName() const
	{
		checkf(GetCppType() == EGESCppType::FName, TEXT("FGESEventDataWrapper:GetString Type:%s isn't a FString"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FName>(Value);
	}

	const FText& GetText() const
	{
		checkf(GetCppType() == EGESCppType::FText, TEXT("FGESEventDataWrapper:GetString Type:%s isn't a FString"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FText>(Value);
	}

	template<typename T=UObject>
	std::enable_if_t<std::is_base_of<UObject, T>::value, T*> GetUObject() const
	{
		checkf(GetCppType() == EGESCppType::UObject, TEXT("FGESEventDataWrapper:GetObject Type:%s isn't a UObject"), *GetTypeString());
		return &FMagicUnion::ConvertToAddr<T>(Value);
	}

	UClass* GetClass() const
	{
		checkf(GetCppType() == EGESCppType::UClass, TEXT("FGESEventDataWrapper:GetClass Type:%s isn't a UClass"), *GetTypeString());
		return &FMagicUnion::ConvertToAddr<UClass>(Value);
	}
	
	template<typename T>
	T& GetStruct() const
	{
		checkf(GetCppType() == EGESCppType::UStruct, TEXT("FGESEventDataWrapper:GetStruct Type:%s isn't a Struct"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T>(Value);
	}

	const void* GetStructRawPtr() const
	{
		checkf(GetCppType() == EGESCppType::UStruct, TEXT("FGESEventDataWrapper:GetStructRawPtr Type:%s isn't a Struct"), *GetTypeString());
		return FMagicUnion::ConvertToRawAddr(Value);
	}

	template<typename T>
	T& GetEnum() const
	{
		checkf(GetCppType() == EGESCppType::UEnum, TEXT("FGESEventDataWrapper:GetEnum Type:%s isn't a Enum"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T>(Value);
	}

	const void* GetEnumRawPtr() const
	{
		checkf(GetCppType() == EGESCppType::UEnum, TEXT("FGESEventDataWrapper:GetEnumRawPtr Type:%s isn't a Enum"), *GetTypeString());
		return FMagicUnion::ConvertToRawAddr(Value);
	}

	template<typename T>
	T* GetAny() const
	{
		checkf(GetCppType() == EGESCppType::Any, TEXT("FGESEventDataWrapper:GetAny Type:%s isn't a AnyObject"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T>(Value);
	}

	template<typename T>
	const TArray<T>& GetArray() const
	{
		checkf(GetContainerType() == EGESContainerType::TArray, TEXT("FGESEventDataWrapper:GetArray Type:%s isn't a TArray"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<TArray<T>>(Value);
	}

	template<typename KeyType, typename ValueType>
	const TMap<KeyType, ValueType>& GetMap() const
	{
		checkf(GetContainerType() == EGESContainerType::TMap, TEXT("FGESEventDataWrapper:GetMap Type:%s isn't a TMap"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<TMap<KeyType, ValueType>>(Value);
	}

	template<typename T>
	const TSet<T>& GetSet() const
	{
		checkf(GetContainerType() == EGESContainerType::TSet, TEXT("FGESEventDataWrapper:GetSet Type:%s isn't a TSet"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<TSet<T>>(Value);
	}

	const void* GetContainerRawPtr() const
	{
		checkf(GetContainerType() == EGESContainerType::TArray ||
			GetContainerType() == EGESContainerType::TMap ||
			GetContainerType() == EGESContainerType::TSet, TEXT("FGESEventDataWrapper:GetContainerRawPtr Type:%s isn't a Container"), *GetTypeString());
		return FMagicUnion::ConvertToRawAddr(Value);
	}

private:
	FString GetTypeString() const
	{
		FString Ret;
		switch (GetCppType())
		{
		case EGESCppType::None:
			{
				switch (GetContainerType())
				{
				case EGESContainerType::None:
					Ret = TEXT("None");
					break;
				case EGESContainerType::TArray:
					Ret = TEXT("Array");
					break;
				case EGESContainerType::TMap:
					Ret = TEXT("Map");
					break;
				case EGESContainerType::TSet:
					Ret = TEXT("Set");
					break;
				}
			}
			break;
		case EGESCppType::Any:
			Ret = TEXT("Any");
			break;
		case EGESCppType::Null:
			Ret = TEXT("Null");
			break;
		case EGESCppType::Bool:
			Ret = TEXT("Bool");
			break;
		case EGESCppType::Integer:
			Ret = TEXT("Integer");
			break;			
		case EGESCppType::Float:
			Ret = TEXT("Float");
			break;
		case EGESCppType::FString:
			Ret = TEXT("FString");
			break;
		case EGESCppType::FName:
			Ret = TEXT("FName");
			break;
		case EGESCppType::FText:
			Ret = TEXT("FText");
			break;
		case EGESCppType::UEnum:
			Ret = TEXT("UEnum");
			break;
		case EGESCppType::UStruct:
			Ret = TEXT("UStruct");
			break;
		case EGESCppType::UObject:
			Ret = TEXT("UObject");
			break;
		case EGESCppType::UClass:
			Ret = TEXT("UClass");
			break;
		}
		return Ret;
	}
};

/**
 * FGESEventData
 *
 * The array of FGESEventDataItem
 */
USTRUCT(BlueprintType)
struct GES_API FGESEventDataArray
{
	GENERATED_BODY()

private:
	FGESEventType EventID;
	
	UPROPERTY()
	TArray<FGESEventData> Params;

	UPROPERTY(Transient)
	FGESEventConfigItem EventConfigItem;
	
public:
	void SetEventID(const FGESEventType& InEventID)
	{
		EventID = InEventID;

		FGESEventConfigHelper::FindEvent(EventID, EventConfigItem);
	}
	
	const FGESEventType& GetEventID() const { return EventID; }

	FGESEventDataArray() {}
	
	~FGESEventDataArray()
	{
		Clear();
	}
	
	int32 GetNum() const { return Params.Num(); }
	
	const FGESEventData& GetParam(const int32 Index) const
	{
		checkf(Params.IsValidIndex(Index), TEXT("GetParam Index:%d is out of params bounds:%d"), Index+1, Params.Num());
		return Params[Index];
	}

	void Clear()
	{
		Params.Empty();
	}

	void PushParamWithType(const FGESEventDataType& InType, const void* InValuePtr)
	{
		PushParamWithType(InType, const_cast<void*>(InValuePtr));
	}

	void PushParamWithType(const FGESEventDataType& InType, void* InValuePtr)
	{
		PushParamInternal(InValuePtr, InType.CppType, InType.ContainerType);
	}

	template<typename T>
	void PushParam(const T& Any)
	{
		PushParamInternal(&Any, EGESCppType::Any);
	}

	template<typename T>
	void PushParam(T& Any)
	{
		PushParamInternal(&Any, EGESCppType::Any);
	}

	template<typename T>
	void PushParam(T&& Any)
	{
		PushParamInternal(&Any, EGESCppType::Any);
	}	

	template<typename T>
	void PushParam(const T* Any)
	{
		PushParamInternal(Any, EGESCppType::Any);
	}

	template<typename T>
	void PushParam(T* Any)
	{
		PushParamInternal(Any, EGESCppType::Any);
	}

	void PushParam(std::nullptr_t NullPtr)
	{
		PushParamInternal(nullptr, EGESCppType::UObject);
	}

	void PushParam(const UObject* Object)
	{
		PushParamInternal(Object, EGESCppType::UObject);
	}

	void PushParam(UObject* Object)
	{
		PushParamInternal(Object, EGESCppType::UObject);
	}

	void PushParam(const UClass* Class)
	{
		PushParamInternal(Class, EGESCppType::UClass);
	}

	void PushParam(UClass* Class)
	{
		PushParamInternal(Class, EGESCppType::UClass);
	}

	void PushParam(bool& bValue)
	{
		PushParamInternal(&bValue, EGESCppType::Bool);
	}	

	void PushParam(const bool& bValue)
	{
		PushParamInternal(&bValue, EGESCppType::Bool);
	}

	void PushParam(int8& ByteValue)
	{
		PushParamInternal(&ByteValue, EGESCppType::Integer);
	}
	
	void PushParam(const int8& ByteValue)
	{
		PushParamInternal(&ByteValue, EGESCppType::Integer);
	}

	void PushParam(int16& ShortValue)
	{
		PushParamInternal(&ShortValue, EGESCppType::Integer);
	}
	
	void PushParam(const int16& ShortValue)
	{
		PushParamInternal(&ShortValue, EGESCppType::Integer);
	}

	void PushParam(int32& IntValue)
	{
		PushParamInternal(&IntValue, EGESCppType::Integer);
	}	
	
	void PushParam(const int32& IntValue)
	{
		PushParamInternal(&IntValue, EGESCppType::Integer);
	}

	void PushParam(int64& LongValue)
	{
		PushParamInternal(&LongValue, EGESCppType::Integer);
	}
	
	void PushParam(const int64& LongValue)
	{
		PushParamInternal(&LongValue, EGESCppType::Integer);
	}

	void PushParam(float& FloatValue)
	{
		PushParamInternal(&FloatValue, EGESCppType::Float);
	}	
	
	void PushParam(const float& FloatValue)
	{
		PushParamInternal(&FloatValue, EGESCppType::Float);
	}

	void PushParam(double& DoubleValue)
	{
		PushParamInternal(&DoubleValue, EGESCppType::Float);
	}	
	
	void PushParam(const double& DoubleValue)
	{
		PushParamInternal(&DoubleValue, EGESCppType::Float);
	}

	void PushParam(const FString& StrValue)
	{
		PushParamInternal(&StrValue, EGESCppType::FString);
	}

	void PushParam(FString& StrValue)
	{
		PushParamInternal(&StrValue, EGESCppType::FString);
	}

	void PushParam(FString&& StrValue)
	{
		PushParamInternal(&StrValue, EGESCppType::FString);
	}	

	void PushParam(const FName& NameValue)
	{
		PushParamInternal(&NameValue, EGESCppType::FName);
	}

	void PushParam(FName& NameValue)
	{
		PushParamInternal(&NameValue, EGESCppType::FName);
	}

	void PushParam(FName&& NameValue)
	{
		PushParamInternal(&NameValue, EGESCppType::FName);
	}

	void PushParam(const FText& TextValue)
	{
		PushParamInternal(&TextValue, EGESCppType::FText);
	}

	void PushParam(FText& TextValue)
	{
		PushParamInternal(&TextValue, EGESCppType::FText);
	}

	void PushParam(FText&& TextValue)
	{
		PushParamInternal(&TextValue, EGESCppType::FText);
	}	

	template<typename T>
	void PushParam(const TArray<T>& ArrayValue)
	{
		PushParamInternal(&ArrayValue, EGESCppType::Any, EGESContainerType::TArray);
	}

	template<typename T>
	void PushParam(TArray<T>& ArrayValue)
	{
		PushParamInternal(&ArrayValue, EGESCppType::Any, EGESContainerType::TArray);
	}

	template<typename T>
	void PushParam(TArray<T>&& ArrayValue)
	{
		PushParamInternal(&ArrayValue, EGESCppType::Any, EGESContainerType::TArray);
	}

	template<typename KeyType, typename ValueType>
	void PushParam(const TMap<KeyType, ValueType>& MapValue)
	{
		PushParamInternal(&MapValue, EGESCppType::Any, EGESContainerType::TMap);
	}

	template<typename KeyType, typename ValueType>
	void PushParam(TMap<KeyType, ValueType>& MapValue)
	{
		PushParamInternal(&MapValue, EGESCppType::Any, EGESContainerType::TMap);
	}

	template<typename KeyType, typename ValueType>
	void PushParam(TMap<KeyType, ValueType>&& MapValue)
	{
		PushParamInternal(&MapValue, EGESCppType::Any, EGESContainerType::TMap);
	}

	template<typename T>
	void PushParam(const TSet<T>& SetValue)
	{
		PushParamInternal(&SetValue, EGESCppType::Any, EGESContainerType::TSet);
	}

	template<typename T>
	void PushParam(TSet<T>& SetValue)
	{
		PushParamInternal(&SetValue, EGESCppType::Any, EGESContainerType::TSet);
	}

	template<typename T>
	void PushParam(TSet<T>&& SetValue)
	{
		PushParamInternal(&SetValue, EGESCppType::Any, EGESContainerType::TSet);
	}	

	void* GetNull(const int32 Index) const
	{
		return GetParam(Index).GetNull();
	}

	bool GetBool(const int32 Index) const
	{
		return GetParam(Index).GetBool();
	}

	int8 GetByte(const int32 Index) const
	{
		return GetParam(Index).GetByte();
	}

	int16 GetShort(const int32 Index) const
	{
		return GetParam(Index).GetShort();
	}

	int32 GetInt(const int32 Index) const
	{
		return GetParam(Index).GetInt();
	}

	int64 GetLong(const int32 Index) const
	{
		return GetParam(Index).GetLong();
	}	

	float GetFloat(const int32 Index) const
	{
		return GetParam(Index).GetFloat();
	}

	double GetDouble(const int32 Index) const
	{
		return GetParam(Index).GetDouble();
	}	

	const FString& GetString(const int32 Index) const
	{
		return GetParam(Index).GetString();
	}

	const FName& GetName(const int32 Index) const
	{
		return GetParam(Index).GetName();
	}

	const FText& GetText(const int32 Index) const
	{
		return GetParam(Index).GetText();
	}

	template<typename T>
	std::enable_if_t<std::is_base_of<UObject, T>::value, T*> GetUObject(const int32 Index) const
	{
		return GetParam(Index).GetUObject<T>();
	}

	UClass* GetClass(const int32 Index) const
	{
		return GetParam(Index).GetClass();
	}	

	template<typename T>
	T& GetStruct(const int32 Index) const
	{
		return GetParam(Index).GetStruct<T>();
	}

	const void* GetStructRawPtr(const int32 Index) const
	{
		return GetParam(Index).GetStructRawPtr();
	}

	template<typename T>
	T& GetEnum(const int32 Index) const
	{
		return GetParam(Index).GetEnum<T>();
	}

	const void* GetEnumRawPtr(const int32 Index) const
	{
		return GetParam(Index).GetEnumRawPtr();
	}	

	template<typename T>
	T* GetAny(const int32 Index) const
	{
		return GetParam(Index).GetAny<T>();
	}
	
	template<typename T>
	T& GetAnyObject(const int32 Index) const
	{
		T* AnyPtr = GetParam(Index).GetAny<T>();
		checkf(AnyPtr, TEXT("FGESEventDataArray:GetAnyObject AnyPtr is null!"));
		return *(AnyPtr);
	}
	
	template<typename T>
	const TArray<T>& GetArray(const int32 Index) const
	{
		return GetParam(Index).GetArray<T>();
	}

	template<typename KeyType, typename ValueType>
	const TMap<KeyType, ValueType>& GetMap(const int32 Index) const
	{
		return GetParam(Index).GetMap<KeyType, ValueType>();
	}

	template<typename T>
	const TSet<T>& GetSet(const int32 Index) const
	{
		return GetParam(Index).GetSet<T>();
	}	

	const void* GetContainerRawPtr(const int32 Index) const
	{
		return GetParam(Index).GetContainerRawPtr();
	}

	const FName& GetParamSubType(const int32 Index) const
	{
		check(EventConfigItem.IsValid());
		check(EventConfigItem.EventDataTypes.IsValidIndex(Index));

		return EventConfigItem.EventDataTypes[Index].CppSubTypeName;
	}

private:
	void PushParamInternal(const void* InValuePtr, const EGESCppType InCppType, const EGESContainerType InContainerType = EGESContainerType::None)
	{
		const int32 Index = Params.Num();
		
		checkf(EventConfigItem.EventDataTypes.IsValidIndex(Index), TEXT("The number of params for event:%s doesn't match!"), *EventID.ToString());

		const FGESEventDataType& Type = EventConfigItem.EventDataTypes[Index];
		const bool bIsTypeValid = (InCppType == EGESCppType::Any || InCppType == Type.CppType) &&
			(InContainerType == EGESContainerType::None || InContainerType == Type.ContainerType);
		checkf(bIsTypeValid, TEXT("The event data type for event:%s doesn't match!"), *EventID.ToString());

		Params.Add({Type, InValuePtr});
	}
};