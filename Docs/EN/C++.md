# C++
## API
* `FGES::RegisterStatic`：Register event for static function
* `FGES::UnregisterStatic`：Unregister event for static function
* `FGES::Register`：Register event for UObject
* `FGES::Unregister`：Unregister event for UObject
* `FGES::RegisterRaw`：Register event for None-UObject
* `FGES::UnregisterRaw`：Unregister event for None-UObject
* `FGES::Dispatch`：Dispatch event to all(C++/Lua/Blueprint if bind)
## Usage
* Bool
``` c++
FGES::Register(TEXT("BoolTestEvent"), this, &AGESTest::OnBoolEvent);
FGES::Unregister(TEXT("BoolTestEvent"), this, &AGESTest::OnBoolEvent);
FGES::Dispatch("BoolTestEvent", true);

void AGESTest::OnBoolEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const bool BoolValue = EventData.GetBool(0);
}
```
* Int
``` c++
FGES::Register(TEXT("IntegerTestEvent"), this, &AGESTest::OnIntegerEvent);
FGES::Unregister(TEXT("IntegerTestEvent"), this, &AGESTest::OnIntegerEvent);
FGES::Dispatch("IntegerTestEvent", 520);

void AGESTest::OnIntegerEvent(const FGESEventDataArray& EventData)
{
    const int32 IntValue = EventData.GetInt(0);
}
```
* Float
``` c++
FGES::Register(TEXT("FloatTestEvent"), this, &AGESTest::OnFloatEvent);
FGES::Unregister(TEXT("FloatTestEvent"), this, &AGESTest::OnFloatEvent);
FGES::Dispatch("FloatTestEvent", 520.1314f);

void AGESTest::OnFloatEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const float FloatValue = EventData.GetFloat(0);
}
```
* String
``` c++
FGES::Register(TEXT("StringTestEvent"), this, &AGESTest::OnStringEvent);
FGES::Unregister(TEXT("StringTestEvent"), this, &AGESTest::OnStringEvent);

FString StrValue = TEXT("StringValue");
FGES::Dispatch("StringTestEvent", StrValue);

void AGESTest::OnStringEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const FString& StringValue = EventData.GetString(0);
}
```
* Name
``` c++
FGES::Register(TEXT("NameTestEvent"), this, &AGESTest::OnNameEvent);
FGES::Unregister(TEXT("NameTestEvent"), this, &AGESTest::OnNameEvent);

FName NameValue = TEXT("NameValue");
FGES::Dispatch("NameTestEvent", NameValue);

void AGESTest::OnNameEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const FName& NameValue = EventData.GetName(0);
}
```
* Text
``` c++
FGES::Register(TEXT("TextTestEvent"), this, &AGESTest::OnTextEvent);
FGES::Unregister(TEXT("TextTestEvent"), this, &AGESTest::OnTextEvent);

FText TextValue = FText::FromString(TEXT("TextValue"));
FGES::Dispatch("TextTestEvent", TextValue);

void AGESTest::OnTextEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const FText& TextValue = EventData.GetText(0);
}
```
* Enum
``` c++
FGES::Register(TEXT("EnumTestEvent"), this, &AGESTest::OnEnumEvent);
FGES::Unregister(TEXT("EnumTestEvent"), this, &AGESTest::OnEnumEvent);

FGES::Dispatch("EnumTestEvent", (int32)EGESCppType::UEnum);

void AGESTest::OnEnumEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const EGESCppType EnumValue = EventData.GetEnum<EGESCppType>(0);
}
```
* Struct
``` c++
FGES::Register(TEXT("StructTestEvent"), this, &AGESTest::OnStructEvent);
FGES::Unregister(TEXT("StructTestEvent"), this, &AGESTest::OnStructEvent);

FVector VectorValue(5, 52, 520);
FGES::Dispatch("StructTestEvent", VectorValue);

void AGESTest::OnStructEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const FVector& StructValue = EventData.GetStruct<FVector>(0);
}
```
* UObject
``` c++
FGES::Register(TEXT("ObjectTestEvent"), this, &AGESTest::OnObjectEvent);
FGES::Unregister(TEXT("ObjectTestEvent"), this, &AGESTest::OnObjectEvent);

FGES::Dispatch("ObjectTestEvent", this);

void AGESTest::OnObjectEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const AGESTest* ObjectValue = EventData.GetUObject<AGESTest>(0);
}
```
* UClass
``` c++
FGES::Register(TEXT("ClassTestEvent"), this, &AGESTest::OnClassEvent);
FGES::Unregister(TEXT("ClassTestEvent"), this, &AGESTest::OnClassEvent);

FGES::Dispatch("ClassTestEvent", StaticClass());

void AGESTest::OnClassEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const UClass* ClassValue = EventData.GetClass(0);
}
```
* TArray
``` c++
FGES::Register(TEXT("ArrayTestEvent"), this, &AGESTest::OnArrayEvent);
FGES::Unregister(TEXT("ArrayTestEvent"), this, &AGESTest::OnArrayEvent);

TArray<int32> ArrayValue({ 520, 521 });
FGES::Dispatch("ArrayTestEvent", ArrayValue);

void AGESTest::OnArrayEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const TArray<int32>& ArrayValue = EventData.GetArray<int32>(0);
}
```
* TSet
``` c++
FGES::Register(TEXT("SetTestEvent"), this, &AGESTest::OnSetEvent);
FGES::Unregister(TEXT("SetTestEvent"), this, &AGESTest::OnSetEvent);

TSet<int32> SetValue({ 520, 521 });
FGES::Dispatch("SetTestEvent", SetValue);

void AGESTest::OnSetEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const TSet<int32> SetValue = EventData.GetSet<int32>(0);
}
```
* TMap
``` c++
FGES::Register(TEXT("MapTestEvent"), this, &AGESTest::OnMapEvent);
FGES::Unregister(TEXT("MapTestEvent"), this, &AGESTest::OnMapEvent);

TMap<int32, FVector> MapValue;
MapValue.Add(520, FVector(1, 10, 100));
MapValue.Add(521, FVector(5, 52, 520));
FGES::Dispatch("MapTestEvent", MapValue);

void AGESTest::OnMapEvent(const FGESEventDataArray& EventData)
{
    // EventData index start from 0
    const TMap<int32, FVector> MapValue = EventData.GetMap<int32, FVector>(0);
}
```
## Notes
* Dispatch Params
> In consideration of performance, FGES::Dispatch only support right value for base type and immediate value  
> eg:  
> `FGES::Dispatch("BoolTestEvent", true);`=============><font color=green>**OK**</font>  
> `FGES::Dispatch("StructTestEvent", FVector(1, 2, 3));`=><font color=red>**Not Ok**</font>
>  
> It will waste extra memory if support this, so I didn't do that. In addition, right value isn't used normally. If you do need transfer a right value(such as function's return value), use a temp variable instead:  
> `FVector VectorValue = GetVector();`  
> `FGES::Dispatch("StructTestEvent", VectorValue);`=====><font color=green>**OK**</font>
* Enum Type
```c++
// Attention：For lua compatibility，enum type should be converted to int manually
FGES::Dispatch("EnumTestEvent", (int32)EGESCppType::UEnum);
```

> Attention：See more [GESTest.cpp](../../GESProj/Source/GESProj/Test/GESTest.cpp)