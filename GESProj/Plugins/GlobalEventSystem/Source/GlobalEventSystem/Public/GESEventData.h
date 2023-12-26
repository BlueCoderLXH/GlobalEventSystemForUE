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
struct GLOBALEVENTSYSTEM_API FGESEventData
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
	
	const EGESEventDataCppType& GetCppType() const { return Type.CppType; }
	
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
		checkf(GetCppType() == EGESEventDataCppType::EDT_Null && !Value, TEXT("FGESEventDataWrapper:GetNull Type:%s isn't a nullptr"), *GetTypeString());
		return GetValueRawPtr();		
	}
	
	bool GetBool() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Bool, TEXT("FGESEventDataWrapper:GetBool Type:%s isn't a bool"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<bool>(Value);
	}

	int8 GetByte() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int8>(Value);
	}

	int16 GetShort() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int16>(Value);;		
	}

	int32 GetInt() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int32>(Value);
	}

	int64 GetLong() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Integer, TEXT("FGESEventDataWrapper:GetInteger Type:%s isn't a int"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<int64>(Value);
	}

	float GetFloat() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Float, TEXT("FGESEventDataWrapper:GetFloat Type:%s isn't a float"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<float>(Value);
	}

	double GetDouble() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Float, TEXT("FGESEventDataWrapper:GetDouble Type:%s isn't a float"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<double>(Value);
	}

	const FString& GetString() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_String, TEXT("FGESEventDataWrapper:GetString Type:%s isn't a FString"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FString>(Value);
	}

	const FName& GetName() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Name, TEXT("FGESEventDataWrapper:GetString Type:%s isn't a FString"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FName>(Value);
	}

	const FText& GetText() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Text, TEXT("FGESEventDataWrapper:GetString Type:%s isn't a FString"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FText>(Value);
	}

	template<typename T=UObject>
	std::enable_if_t<std::is_base_of<UObject, T>::value, T*> GetUObject() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Object, TEXT("FGESEventDataWrapper:GetObject Type:%s isn't a UObject"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T*>(Value);
	}

	const FSoftObjectPtr& GetSoftObject() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_SoftObject ||
			GetCppType() == EGESEventDataCppType::EDT_SoftClass, TEXT("FGESEventDataWrapper:GetSoftObject Type:%s isn't a SoftObject/SoftClass"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<FSoftObjectPtr>(Value);		
	}

	UClass* GetClass() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Class, TEXT("FGESEventDataWrapper:GetClass Type:%s isn't a UClass"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<UClass*>(Value);
	}
	
	template<typename T>
	T& GetStruct() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Struct, TEXT("FGESEventDataWrapper:GetStruct Type:%s isn't a Struct"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T>(Value);
	}

	template<typename T>
	T& GetEnum() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Enum, TEXT("FGESEventDataWrapper:GetEnum Type:%s isn't a Enum"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T>(Value);
	}

	template<typename T>
	T* GetAny() const
	{
		checkf(GetCppType() == EGESEventDataCppType::EDT_Any, TEXT("FGESEventDataWrapper:GetAny Type:%s isn't a Enum"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<T>(Value);
	}

	template<typename T>
	const TArray<T>& GetArray() const
	{
		checkf(GetContainerType() == EGESContainerType::EDCT_Array, TEXT("FGESEventDataWrapper:GetArray Type:%s isn't a TArray"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<TArray<T>>(Value);
	}

	template<typename KeyType, typename ValueType>
	const TMap<KeyType, ValueType>& GetMap() const
	{
		checkf(GetContainerType() == EGESContainerType::EDCT_Map, TEXT("FGESEventDataWrapper:GetMap Type:%s isn't a TMap"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<TMap<KeyType, ValueType>>(Value);
	}

	template<typename T>
	const TSet<T>& GetSet() const
	{
		checkf(GetContainerType() == EGESContainerType::EDCT_Set, TEXT("FGESEventDataWrapper:GetMap Type:%s isn't a Set"), *GetTypeString());
		return FMagicUnion::ConvertToAddr<TSet<T>>(Value);
	}

	const void* GetContainerRawPtr() const
	{
		checkf(GetContainerType() == EGESContainerType::EDCT_Array ||
			GetContainerType() == EGESContainerType::EDCT_Map ||
			GetContainerType() == EGESContainerType::EDCT_Set, TEXT("FGESEventDataWrapper:GetArray Type:%s isn't a TArray"), *GetTypeString());
		return FMagicUnion::ConvertToRawAddr(Value);
	}

private:
	FString GetTypeString() const
	{
		FString Ret;
		switch (GetCppType())
		{
		case EGESEventDataCppType::EDT_None:
			{
				switch (GetContainerType())
				{
				case EGESContainerType::EDCT_None:
					Ret = TEXT("None");
					break;
				case EGESContainerType::EDCT_Array:
					Ret = TEXT("Array");
					break;
				case EGESContainerType::EDCT_Map:
					Ret = TEXT("Map");
					break;
				case EGESContainerType::EDCT_Set:
					Ret = TEXT("Set");
					break;
				}
			}
			break;
		case EGESEventDataCppType::EDT_Any:
			Ret = TEXT("Any");
			break;
		case EGESEventDataCppType::EDT_Null:
			Ret = TEXT("Null");
			break;
		case EGESEventDataCppType::EDT_Bool:
			Ret = TEXT("Bool");
			break;
		case EGESEventDataCppType::EDT_Integer:
			Ret = TEXT("Integer");
			break;			
		case EGESEventDataCppType::EDT_Float:
			Ret = TEXT("Float");
			break;
		case EGESEventDataCppType::EDT_String:
			Ret = TEXT("FString");
			break;
		case EGESEventDataCppType::EDT_Name:
			Ret = TEXT("FName");
			break;
		case EGESEventDataCppType::EDT_Text:
			Ret = TEXT("FText");
			break;
		case EGESEventDataCppType::EDT_Enum:
			Ret = TEXT("UEnum");
			break;
		case EGESEventDataCppType::EDT_Struct:
			Ret = TEXT("UStruct");
			break;
		case EGESEventDataCppType::EDT_Object:
			Ret = TEXT("UObject");
			break;
		case EGESEventDataCppType::EDT_SoftObject:
			Ret = TEXT("FSoftObject");
			break;
		case EGESEventDataCppType::EDT_Class:
			Ret = TEXT("UClass");
			break;
		case EGESEventDataCppType::EDT_SoftClass:
			Ret = TEXT("FSoftClass");
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
struct GLOBALEVENTSYSTEM_API FGESEventDataArray
{
	GENERATED_BODY()

private:
	FGESEventType EventID;
	
	UPROPERTY()
	TArray<FGESEventData> Params;

	UPROPERTY(Transient)
	FGESEventConfigItem EventConfigItem;
	
public:
	void SetEventID(const FGESEventType& InEventID) { EventID = InEventID; }
	const FGESEventType& GetEventID() const { return EventID; }
	
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
		Params.Add({InType, InValuePtr});
	}

	// template<typename T>
	// void PushParam(const T&& RRefValue)
	// {
	// 	PushParam(const_cast<T&&>(RRefValue));
	// }
	//
	// template<typename T>
	// void PushParam(T&& RRefValue)
	// {
	// 	checkf(0, TEXT("Right value isn't supported, try using temp variable"));
	// }

	template<typename T>
	void PushParam(const T& Any)
	{
		PushParamInternal(&Any);
	}

	template<typename T>
	void PushParam(T& Any)
	{
		PushParamInternal(&Any);
	}

	template<typename T>
	void PushParam(T&& Any)
	{
		PushParamInternal(&Any);
	}	

	template<typename T>
	void PushParam(const T* Any)
	{
		PushParamInternal(Any);
	}

	template<typename T>
	void PushParam(T* Any)
	{
		PushParamInternal(Any);
	}

	void PushParam(std::nullptr_t NullPtr)
	{
		PushParamInternal(nullptr);
	}

	void PushParam(const UObject* Object)
	{
		PushParamInternal(Object);
	}

	void PushParam(UObject* Object)
	{
		PushParamInternal(Object);
	}

	void PushParam(const bool& bValue)
	{
		PushParamInternal(&bValue);
	}

	void PushParam(const int8& ByteValue)
	{
		PushParamInternal(&ByteValue);
	}
	
	void PushParam(const int16& ShortValue)
	{
		PushParamInternal(&ShortValue);
	}

	void PushParam(const int32& IntValue)
	{
		PushParamInternal(&IntValue);
	}

	void PushParam(const int64& LongValue)
	{
		PushParamInternal(&LongValue);
	}

	void PushParam(const float& FloatValue)
	{
		PushParamInternal(&FloatValue);
	}	
	
	void PushParam(const double& DoubleValue)
	{
		PushParamInternal(&DoubleValue);
	}

	void PushParam(const FString& StrValue)
	{
		PushParamInternal(&StrValue);
	}

	void PushParam(FString& StrValue)
	{
		PushParamInternal(&StrValue);
	}

	void PushParam(FString&& StrValue)
	{
		PushParamInternal(&StrValue);
	}	

	void PushParam(const FName& NameValue)
	{
		PushParamInternal(&NameValue);
	}

	void PushParam(FName& NameValue)
	{
		PushParamInternal(&NameValue);
	}

	void PushParam(FName&& NameValue)
	{
		PushParamInternal(&NameValue);
	}

	void PushParam(const FText& TextValue)
	{
		PushParamInternal(&TextValue);
	}

	void PushParam(FText& TextValue)
	{
		PushParamInternal(&TextValue);
	}

	void PushParam(FText&& TextValue)
	{
		PushParamInternal(&TextValue);
	}	

	template<typename T>
	void PushParam(const TArray<T>& ArrayValue)
	{
		PushParamInternal(&ArrayValue);
	}

	template<typename T>
	void PushParam(TArray<T>& ArrayValue)
	{
		PushParamInternal(&ArrayValue);
	}

	template<typename T>
	void PushParam(TArray<T>&& SetValue)
	{
		PushParamInternal(&SetValue);
	}

	template<typename KeyType, typename ValueType>
	void PushParam(const TMap<KeyType, ValueType>& MapValue)
	{
		PushParamInternal(&MapValue);
	}

	template<typename KeyType, typename ValueType>
	void PushParam(TMap<KeyType, ValueType>& MapValue)
	{
		PushParamInternal(&MapValue);
	}

	template<typename KeyType, typename ValueType>
	void PushParam(TMap<KeyType, ValueType>&& MapValue)
	{
		PushParamInternal(&MapValue);
	}

	template<typename T>
	void PushParam(const TSet<T>& SetValue)
	{
		PushParamInternal(&SetValue);
	}

	template<typename T>
	void PushParam(TSet<T>& SetValue)
	{
		PushParamInternal(&SetValue);
	}

	template<typename T>
	void PushParam(TSet<T>&& SetValue)
	{
		PushParamInternal(&SetValue);
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

	const void* GetArrayRawPtr(const int32 Index) const
	{
		return GetParam(Index).GetContainerRawPtr();
	}

private:
	void PushParamInternal(const void* InValuePtr)
	{
		const int32 Index = Params.Num();
		if (Index == 0)
		{
			UGESEventConfigHelper::FindEvent(EventID, EventConfigItem);
		}

		checkf(EventConfigItem.EventDataTypes.IsValidIndex(Index), TEXT("The number of params for event:%s don't match!"), *EventID.ToString());

		const FGESEventDataType& Type = EventConfigItem.EventDataTypes[Index];
		Params.Add({Type, InValuePtr});
	}
};