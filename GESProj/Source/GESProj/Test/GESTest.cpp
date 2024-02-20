// Fill out your copyright notice in the Description page of Project Settings.


#include "GESTest.h"

AGESTest::AGESTest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGESTest::BeginPlay()
{
	Super::BeginPlay();

	if (bDoCppTest_Register)
	{
		DoCppTest_Register();
	}

	if (bDoCppTest_Dispatch)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			DoCppTest_Dispatch();
		});
	}

	if (bDoBPTest_Register)
	{
		DoBPTest_Register();
	}

	if (bDoBPTest_Dispatch)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			DoBPTest_Dispatch();
		});
	}

	if (bDoLuaTest_Register)
	{
		DoLuaTest_Register();
	}

	if (bDoLuaTest_Dispatch)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			DoLuaTest_Dispatch();
		});
	}
}

void AGESTest::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (bDoCppTest_Register)
	{
		DoCppTest_Unregister();
	}

	if (bDoBPTest_Register)
	{
		DoCppTest_Unregister();
	}

	if (bDoLuaTest_Register)
	{
		DoCppTest_Unregister();
	}
}

void AGESTest::DoCppTest_Register()
{
	FGES::Register(TEXT("BoolTestEvent"), this, &AGESTest::OnBoolEvent);
	FGES::Register(TEXT("IntegerTestEvent"), this, &AGESTest::OnIntegerEvent);
	FGES::Register(TEXT("FloatTestEvent"), this, &AGESTest::OnFloatEvent);
	FGES::Register(TEXT("StringTestEvent"), this, &AGESTest::OnStringEvent);
	FGES::Register(TEXT("NameTestEvent"), this, &AGESTest::OnNameEvent);
	FGES::Register(TEXT("TextTestEvent"), this, &AGESTest::OnTextEvent);
	FGES::Register(TEXT("EnumTestEvent"), this, &AGESTest::OnEnumEvent);
	FGES::Register(TEXT("StructTestEvent"), this, &AGESTest::OnStructEvent);
	FGES::Register(TEXT("ObjectTestEvent"), this, &AGESTest::OnObjectEvent);
	FGES::Register(TEXT("ClassTestEvent"), this, &AGESTest::OnClassEvent);
	FGES::Register(TEXT("ArrayTestEvent"), this, &AGESTest::OnArrayEvent);
	FGES::Register(TEXT("SetTestEvent"), this, &AGESTest::OnSetEvent);
	FGES::Register(TEXT("MapTestEvent"), this, &AGESTest::OnMapEvent);
}

void AGESTest::DoCppTest_Unregister()
{
	FGES::Unregister(TEXT("BoolTestEvent"), this, &AGESTest::OnBoolEvent);
	FGES::Unregister(TEXT("IntegerTestEvent"), this, &AGESTest::OnIntegerEvent);
	FGES::Unregister(TEXT("FloatTestEvent"), this, &AGESTest::OnFloatEvent);
	FGES::Unregister(TEXT("StringTestEvent"), this, &AGESTest::OnStringEvent);
	FGES::Unregister(TEXT("NameTestEvent"), this, &AGESTest::OnNameEvent);
	FGES::Unregister(TEXT("TextTestEvent"), this, &AGESTest::OnTextEvent);
	FGES::Unregister(TEXT("EnumTestEvent"), this, &AGESTest::OnEnumEvent);
	FGES::Unregister(TEXT("StructTestEvent"), this, &AGESTest::OnStructEvent);
	FGES::Unregister(TEXT("ObjectTestEvent"), this, &AGESTest::OnObjectEvent);
	FGES::Unregister(TEXT("ClassTestEvent"), this, &AGESTest::OnClassEvent);
	FGES::Unregister(TEXT("ArrayTestEvent"), this, &AGESTest::OnArrayEvent);
	FGES::Unregister(TEXT("SetTestEvent"), this, &AGESTest::OnSetEvent);
	FGES::Unregister(TEXT("MapTestEvent"), this, &AGESTest::OnMapEvent);
}

void AGESTest::DoCppTest_Dispatch()
{
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] ========================== AGESTest::DoTest_Dispatch =========================="));
	
	FGES::Dispatch("BoolTestEvent", true);
	FGES::Dispatch("IntegerTestEvent", 520);
	FGES::Dispatch("FloatTestEvent", 520.1314f);

	FString StrValue = TEXT("StringValue");
	FGES::Dispatch("StringTestEvent", StrValue);

	FName NameValue = TEXT("NameValue");
	FGES::Dispatch("NameTestEvent", NameValue);

	FText TextValue = FText::FromString(TEXT("TextValue"));
	FGES::Dispatch("TextTestEvent", TextValue);

	FGES::Dispatch("EnumTestEvent", (int32)EGESCppType::UEnum);

	FVector VectorValue(5, 52, 520);
	FGES::Dispatch("StructTestEvent", VectorValue);

	FGES::Dispatch("ObjectTestEvent", this);
	FGES::Dispatch("ClassTestEvent", StaticClass());

	TArray<int32> ArrayValue({ 520, 521 });
	FGES::Dispatch("ArrayTestEvent", ArrayValue);
	
	TSet<int32> SetValue({ 520, 521 });
	FGES::Dispatch("SetTestEvent", SetValue);
	
	TMap<int32, FVector> MapValue;
	MapValue.Add(520, FVector(1, 10, 100));
	MapValue.Add(521, FVector(5, 52, 520));
	FGES::Dispatch("MapTestEvent", MapValue);

	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] ========================== AGESTest::DoTest_Dispatch =========================="));
}

void AGESTest::OnBoolEvent(const FGESEventDataArray& EventData)
{
	const bool BoolValue = EventData.GetBool(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnBoolEvent Value:%s"), BoolValue ? TEXT("True") : TEXT("False"));
}

void AGESTest::OnIntegerEvent(const FGESEventDataArray& EventData)
{
	const int32 IntValue = EventData.GetInt(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnIntegerEvent Value:%d"), IntValue);
}

void AGESTest::OnFloatEvent(const FGESEventDataArray& EventData)
{
	const float FloatValue = EventData.GetFloat(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnFloatEvent Value:%f"), FloatValue);	
}

void AGESTest::OnStringEvent(const FGESEventDataArray& EventData)
{
	const FString& StringValue = EventData.GetString(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnStringEvent Value:%s"), *StringValue);	
}

void AGESTest::OnNameEvent(const FGESEventDataArray& EventData)
{
	const FName& NameValue = EventData.GetName(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnNameEvent Value:%s"), *NameValue.ToString());	
}

void AGESTest::OnTextEvent(const FGESEventDataArray& EventData)
{
	const FText& TextValue = EventData.GetText(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnTextEvent Value:%s"), *TextValue.ToString());	
}

void AGESTest::OnEnumEvent(const FGESEventDataArray& EventData)
{
	const EGESCppType EnumValue = EventData.GetEnum<EGESCppType>(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnEnumEvent Value:%d"), EnumValue);	
}

void AGESTest::OnStructEvent(const FGESEventDataArray& EventData)
{
	const FVector& StructValue = EventData.GetStruct<FVector>(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnStructEvent Value:%s"), *StructValue.ToString());
}

void AGESTest::OnObjectEvent(const FGESEventDataArray& EventData)
{
	const AGESTest* ObjectValue = EventData.GetUObject<AGESTest>(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnObjectEvent Value:%s"), *ObjectValue->GetName());
}

void AGESTest::OnClassEvent(const FGESEventDataArray& EventData)
{
	const UClass* ClassValue = EventData.GetClass(0);
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnClassEvent Value:%s"), *ClassValue->GetName());
}

void AGESTest::OnArrayEvent(const FGESEventDataArray& EventData)
{
	const TArray<int32>& ArrayValue = EventData.GetArray<int32>(0);
	FString Ret = TEXT("{ ");
	for (const int32 Item : ArrayValue)
	{
		Ret += FString::Printf(TEXT("%d "), Item);
	}
	Ret += TEXT("}");
	
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnArrayEvent Value:%s"), *Ret);
}

void AGESTest::OnSetEvent(const FGESEventDataArray& EventData)
{
	const TSet<int32> SetValue = EventData.GetSet<int32>(0);
	FString Ret = TEXT("{ ");
	for (const int32 Item : SetValue)
	{
		Ret += FString::Printf(TEXT("%d "), Item);
	}
	Ret += TEXT("}");
	
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnSetEvent Value:%s"), *Ret);
}

void AGESTest::OnMapEvent(const FGESEventDataArray& EventData)
{
	const TMap<int32, FVector> MapValue = EventData.GetMap<int32, FVector>(0);

	FString Ret = TEXT("{ ");
	for (const auto& Item : MapValue)
	{
		Ret += FString::Printf(TEXT("{ K:%d, V:(%s) } "), Item.Key, *Item.Value.ToString());
	}
	Ret += TEXT("}");
	
	UE_LOG(LogTemp, Log, TEXT("[GESTest_CPP] AGESTest::OnMapEvent Value:%s"), *Ret);
}

void AGESTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

