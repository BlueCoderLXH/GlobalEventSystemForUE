// Fill out your copyright notice in the Description page of Project Settings.

#include "GESBPLibrary.h"

#include "Engine.h"
#include "GESEditorCommon.h"
#include "GlobalEventSystem.h"

using namespace GESEditorConstant;

void UGESBPLibrary::GESDispatchEvent(FName EventType)
{
	check(0);
}

DEFINE_FUNCTION(UGESBPLibrary::execGESDispatchEvent)
{
	P_GET_PROPERTY(FNameProperty, Z_Param_EventType);

	UE_LOG(LogGlobalEventSystem, Verbose, TEXT("DispatchEvent EventType:%s"), *Z_Param_EventType.ToString());

	FGESEventDataArray EventData;
	EventData.SetEventID(Z_Param_EventType);

	FGESEventConfigItem EventConfig;
	UGESEventConfigHelper::FindEvent(Z_Param_EventType, EventConfig);
	
	int32 EventDataIndex = -1;
	while (++EventDataIndex < EventConfig.EventDataTypes.Num())
	{
		Stack.MostRecentProperty = nullptr;
		Stack.MostRecentPropertyAddress = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);

		if (!Stack.MostRecentProperty || !Stack.MostRecentPropertyAddress) break;

		P_NATIVE_BEGIN;
		GESDispatchEventInternal(Stack.MostRecentProperty, Stack.MostRecentPropertyAddress, EventConfig.EventDataTypes[EventDataIndex], EventData);
		P_NATIVE_END;
	}

	P_FINISH;

	FGlobalEventSystem::DispatchFromBP(EventData);
}

void UGESBPLibrary::GESDispatchEventInternal(FProperty* PropertyPtr, const void* DataPtr, const FGESEventDataType& Type, FGESEventDataArray& EventDataArray)
{
	EGESCppType TargetEventDataType = EGESCppType::None;
	EGESContainerType TargetContainerType = EGESContainerType::None;

	if (CastField<FArrayProperty>(PropertyPtr))
	{
		TargetEventDataType = Type.CppType;
		TargetContainerType = EGESContainerType::Array;
	}
	else if (CastField<FMapProperty>(PropertyPtr))
	{
		TargetEventDataType = Type.CppType;
		TargetContainerType = EGESContainerType::Map;
	}
	else if (CastField<FSetProperty>(PropertyPtr))
	{
		TargetEventDataType = Type.CppType;
		TargetContainerType = EGESContainerType::Set;
	}
	else
	{
		if (CastField<FBoolProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::Bool;
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
		else if (CastField<FEnumProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UEnum;
		}
		else if (CastField<FStructProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UStruct;
		}
		else if (CastField<FObjectProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UObject;
		}
		else if (CastField<FClassProperty>(PropertyPtr))
		{
			TargetEventDataType = EGESCppType::UClass;
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
	FGlobalEventSystem::RegisterBP(EventType, EventDelegate.GetUObject(), EventDelegate.GetFunctionName());
}

void UGESBPLibrary::GESUnregisterEvent(FName EventType)
{
	check(0);
}

DEFINE_FUNCTION(UGESBPLibrary::execGESUnregisterEvent)
{
	P_GET_PROPERTY_REF(FNameProperty, Z_Param_EventName);

	check(Z_Param_EventName.IsNone());

	const UObject* DelegateOwner = Stack.Object;
	check(IsValid(DelegateOwner));

	FGlobalEventSystem::UnregisterBP(Z_Param_EventName, DelegateOwner, GetCustomEventFuncName(Z_Param_EventName));
	
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
				UClass* ClassValue = EventData.GetUObject<UClass>();
				P_GET_PROPERTY_REF(FClassProperty, Z_Param_Class);
				Z_Param_Class = ClassValue;
			}
		}
		else
		{
			Stack.MostRecentProperty = nullptr;
			Stack.MostRecentPropertyAddress = nullptr;

			const FProperty* ContainerProperty = nullptr;
			
			if (ContainerType == EGESContainerType::Array)
			{
				Stack.StepCompiledIn<FArrayProperty>(nullptr);
				ContainerProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
			}
			else if (ContainerType == EGESContainerType::Map)
			{
				Stack.StepCompiledIn<FMapProperty>(nullptr);
				ContainerProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
			}
			else if (ContainerType == EGESContainerType::Set)
			{
				Stack.StepCompiledIn<FSetProperty>(nullptr);
				ContainerProperty = CastField<FSetProperty>(Stack.MostRecentProperty);
			}

			void* ContainerDataPtr = Stack.MostRecentPropertyAddress;
			check(ContainerProperty && ContainerDataPtr);

			const void* TargetArrayDataPtr = EventData.GetContainerRawPtr();
			ContainerProperty->CopyCompleteValue(ContainerDataPtr, TargetArrayDataPtr);			
		}
	}

	P_FINISH;
}

void UGESBPLibrary::RegisterEventLua(const FName EventType, FGESDelegateForBP EventDelegate)
{
	FGlobalEventSystem::RegisterBP(EventType, EventDelegate.GetUObject(), EventDelegate.GetFunctionName());	
}

void UGESBPLibrary::DispatchEventLua(const FGESEventDataArray& EventData)
{
	FGlobalEventSystem::DispatchFromBP(EventData);
}

void UGESBPLibrary::UnregisterEventLua(const FName EventType, FGESDelegateForBP EventDelegate)
{
	FGlobalEventSystem::UnregisterBP(EventType, EventDelegate.GetUObject(), EventDelegate.GetFunctionName());
}
