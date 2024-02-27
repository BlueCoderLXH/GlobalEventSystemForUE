// Fill out your copyright notice in the Description page of Project Settings.

#include "GESBPLibrary.h"

#include "Engine.h"
#include "GES.h"

TAutoConsoleVariable<bool> CVarGESHotReloadConfig(
	TEXT("ges.hotreload"),
	false,
	TEXT("Whether should hot reload ges cpp event config from lua.\n")
	TEXT("  true = Hot reload ges config every time in editor\n")
	TEXT("  false = Not hot reload"),
	ECVF_Default);

void UGESBPLibrary::GESDispatchEvent(FName EventType)
{
	check(0);
}

DEFINE_FUNCTION(UGESBPLibrary::execGESDispatchEvent)
{
	P_GET_PROPERTY(FNameProperty, Z_Param_EventType);

	UE_LOG(LogGES, Verbose, TEXT("DispatchEvent EventType:%s"), *Z_Param_EventType.ToString());

	FGESEventDataArray EventData;
	EventData.SetEventID(Z_Param_EventType);

	FGESEventConfigItem EventConfig;
	FGESEventConfigHelper::FindEvent(Z_Param_EventType, EventConfig);

	const TArray<FGESEventDataType>& EventDataTypes = EventConfig.EventDataTypes;
	
	int32 EventDataIndex = -1;
	while (++EventDataIndex < EventDataTypes.Num())
	{
		const FGESEventDataType& EventDataType = EventDataTypes[EventDataIndex];
		
		// Object(include Class) should use this way to get it's value ptr
		if (EventDataType.CppType == EGESCppType::UObject)
		{
			P_GET_PROPERTY(FObjectProperty, ObjectValue);
			EventData.PushParamWithType(EventDataType, ObjectValue);
		}
		else if (EventDataType.CppType == EGESCppType::UClass)
		{
			P_GET_PROPERTY(FClassProperty, ClassValue);
			EventData.PushParamWithType(EventDataType, ClassValue);
		}
		// Otherwise, get it directly and check type's validation
		else
		{
			Stack.MostRecentProperty = nullptr;
			Stack.MostRecentPropertyAddress = nullptr;
			Stack.StepCompiledIn<FProperty>(nullptr);

			if (!Stack.MostRecentProperty || !Stack.MostRecentPropertyAddress) break;

			P_NATIVE_BEGIN;
			GESDispatchEventInternal(Stack.MostRecentProperty, Stack.MostRecentPropertyAddress, EventDataType, EventData);
			P_NATIVE_END;
		}
	}

	P_FINISH;

	FGES::DispatchBP(EventData);
}

void UGESBPLibrary::GESDispatchEventInternal(FProperty* PropertyPtr, const void* DataPtr, const FGESEventDataType& Type, FGESEventDataArray& EventDataArray)
{
	EGESCppType TargetEventDataType = EGESCppType::None;
	EGESContainerType TargetContainerType = EGESContainerType::None;

	if (CastField<FArrayProperty>(PropertyPtr))
	{
		TargetEventDataType = Type.CppType;
		TargetContainerType = EGESContainerType::TArray;
	}
	else if (CastField<FMapProperty>(PropertyPtr))
	{
		TargetEventDataType = Type.CppType;
		TargetContainerType = EGESContainerType::TMap;
	}
	else if (CastField<FSetProperty>(PropertyPtr))
	{
		TargetEventDataType = Type.CppType;
		TargetContainerType = EGESContainerType::TSet;
	}
	else
	{
		if (CastField<FBoolProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::Bool;
		}
		// 'FByteProperty' inherits from 'FNumericProperty', so judge it before 'FNumericProperty' to make 'UEnum' type okay
		else if (CastField<FEnumProperty>(PropertyPtr) || CastField<FByteProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UEnum;
		}
		else if (const FNumericProperty* NumberProperty = CastField<FNumericProperty>(PropertyPtr))
		{
			if (NumberProperty->IsInteger())
			{
				TargetEventDataType = EGESCppType::Integer;
			}
			else if (NumberProperty->IsFloatingPoint())
			{
				TargetEventDataType = EGESCppType::Float;
			}
		}
		else if (CastField<FStrProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::FString;
		}
		else if (CastField<FNameProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::FName;
		}
		else if (CastField<FTextProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::FText;
		}
		else if (CastField<FStructProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UStruct;
		}
		// 'FClassProperty' inherits from 'FObjectProperty', so judge it before 'FObjectProperty' to make 'UClass' type okay
		else if (CastField<FClassProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UClass;
		}
		else if (CastField<FObjectProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UObject;
		}

		checkf(TargetEventDataType != EGESCppType::None,
			TEXT("[GESDispatchEventInternal] EventDataType:None is unsupported EventType:%s"), *EventDataArray.GetEventID().ToString());	
	}

	checkf(TargetEventDataType == Type.CppType && TargetContainerType == Type.ContainerType,
		TEXT("[GESDispatchEventInternal] EventDataType doesn't match for %s"), *EventDataArray.GetEventID().ToString());

	EventDataArray.PushParamWithType(Type, DataPtr);
}

void UGESBPLibrary::GESRegisterEvent(FName EventType, FGESDelegateForBP EventDelegate)
{
	FGES::RegisterBP(EventType, EventDelegate.GetUObject(), EventDelegate.GetFunctionName());
}

void UGESBPLibrary::GESUnregisterEvent(FName EventType)
{
	check(0);
}

DEFINE_FUNCTION(UGESBPLibrary::execGESUnregisterEvent)
{
	P_GET_PROPERTY_REF(FNameProperty, Z_Param_EventName);

	check(!Z_Param_EventName.IsNone());

	const UObject* DelegateOwner = Stack.Object;
	check(IsValid(DelegateOwner));

	FGES::UnregisterBP(Z_Param_EventName, DelegateOwner, GetCustomEventFuncName(Z_Param_EventName));
	
	P_FINISH;
}

void UGESBPLibrary::GESConvertEventData(FGESEventDataArray& EventDataArray)
{
	check(0);
}
DEFINE_FUNCTION(UGESBPLibrary::execGESConvertEventData)
{
	P_GET_STRUCT_REF(FGESEventDataArray, EventDataArray);

	for (int32 Index = 0; Index < EventDataArray.GetNum(); Index++)
	{
		const FGESEventData& EventData = EventDataArray.GetParam(Index);
		const EGESCppType CppType = EventData.GetCppType();
		const EGESContainerType ContainerType = EventData.GetContainerType();
		if (ContainerType == EGESContainerType::None)
		{
			if (CppType == EGESCppType::Bool)
			{
				const bool BoolValue = EventData.GetBool();
				P_GET_PROPERTY_REF(FBoolProperty, Z_Param_Bool);
				Z_Param_Bool = BoolValue;
			}
			else if (CppType == EGESCppType::Integer)
			{
				const int32 IntValue = EventData.GetInt();
				P_GET_PROPERTY_REF(FIntProperty, Z_Param_Int);
				Z_Param_Int = IntValue;
			}
			else if (CppType == EGESCppType::Float)
			{
				const float FloatValue = EventData.GetFloat();
				P_GET_PROPERTY_REF(FFloatProperty, Z_Param_Float);
				Z_Param_Float = FloatValue;
			}
			else if (CppType == EGESCppType::FString)
			{
				const FString& StrValue = EventData.GetString();
				P_GET_PROPERTY_REF(FStrProperty, Z_Param_String);
				Z_Param_String = StrValue;
			}
			else if (CppType == EGESCppType::FName)
			{
				const FName& NameValue = EventData.GetName();
				P_GET_PROPERTY_REF(FNameProperty, Z_Param_Name);
				Z_Param_Name = NameValue;
			}
			else if (CppType == EGESCppType::FText)
			{
				const FText& TextValue = EventData.GetText();
				P_GET_PROPERTY_REF(FTextProperty, Z_Param_Text);
				Z_Param_Text = TextValue;
			}
			else if (CppType == EGESCppType::UEnum)
			{
				Stack.StepCompiledIn<FByteProperty>(nullptr);

				const FEnumProperty* OutEnumProp = CastField<FEnumProperty>(Stack.MostRecentProperty);
				uint8* OutEnumDataPtr = Stack.MostRecentPropertyAddress;
				check(OutEnumProp && OutEnumDataPtr);

				const void* SourceDataRawPtr = EventData.GetValueRawPtr();

				OutEnumProp->CopyCompleteValue(OutEnumDataPtr, SourceDataRawPtr);
			}
			else if (CppType == EGESCppType::UStruct)
			{
				Stack.StepCompiledIn<FStructProperty>(nullptr);
				
				const FStructProperty* OutStructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
				uint8* OutStructDataPtr = Stack.MostRecentPropertyAddress;
				check(OutStructProp && OutStructDataPtr);

				const void* SourceDataRawPtr = EventData.GetValueRawPtr();

				OutStructProp->CopyCompleteValue(OutStructDataPtr, SourceDataRawPtr);
			}
			else if (CppType == EGESCppType::UObject)
			{
				UObject* ObjValue = EventData.GetUObject();
				P_GET_PROPERTY_REF(FObjectProperty, Z_Param_Object);
				Z_Param_Object = ObjValue;
			}
			else if (CppType == EGESCppType::UClass)
			{
				UClass* ClassValue = EventData.GetClass();
				P_GET_PROPERTY_REF(FClassProperty, Z_Param_Class);
				Z_Param_Class = ClassValue;
			}
			else
			{
				checkf(0, TEXT("UGESBPLibrary::execGESConvertEventData Unsupported cpp type:%d"), CppType);
			}
		}
		else
		{
			Stack.MostRecentProperty = nullptr;
			Stack.MostRecentPropertyAddress = nullptr;

			const FProperty* ContainerProperty = nullptr;
			
			if (ContainerType == EGESContainerType::TArray)
			{
				Stack.StepCompiledIn<FArrayProperty>(nullptr);
				ContainerProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
			}
			else if (ContainerType == EGESContainerType::TMap)
			{
				Stack.StepCompiledIn<FMapProperty>(nullptr);
				ContainerProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
			}
			else if (ContainerType == EGESContainerType::TSet)
			{
				Stack.StepCompiledIn<FSetProperty>(nullptr);
				ContainerProperty = CastField<FSetProperty>(Stack.MostRecentProperty);
			}
			else
			{
				checkf(0, TEXT("UGESBPLibrary::execGESConvertEventData Unsupported container type:%d"), ContainerType);
			}

			void* ContainerDataPtr = Stack.MostRecentPropertyAddress;
			check(ContainerProperty && ContainerDataPtr);

			const void* TargetArrayDataPtr = EventData.GetContainerRawPtr();
			ContainerProperty->CopyCompleteValue(ContainerDataPtr, TargetArrayDataPtr);			
		}
	}

	P_FINISH;
}

void UGESBPLibrary::GESEmptyConnection()
{
	check(0);
}
DEFINE_FUNCTION(UGESBPLibrary::execGESEmptyConnection)
{
	// 'GESEmptyConnection' may have some input params, traverse all of them to make bp vm loop fine
	while (true)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.MostRecentPropertyAddress = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);

		if (!Stack.MostRecentProperty || !Stack.MostRecentPropertyAddress) break;
	}
	
	P_FINISH;
}


void UGESBPLibrary::RegisterLua(const FName EventType, FGESDelegateForBP EventDelegate)
{
	FGES::RegisterBP(EventType, EventDelegate.GetUObject(), EventDelegate.GetFunctionName());	
}

void UGESBPLibrary::DispatchLua(const FGESEventDataArray& EventData)
{
	FGES::DispatchBP(EventData);
}

void UGESBPLibrary::UnregisterLua(const FName EventType, FGESDelegateForBP EventDelegate)
{
	FGES::UnregisterBP(EventType, EventDelegate.GetUObject(), EventDelegate.GetFunctionName());
}