# C++
## API
* `FGES::RegisterStatic`：注册事件（静态函数）
* `FGES::UnregisterStatic`：反注册事件（静态函数）
* `FGES::Register`：注册事件（UObject）
* `FGES::Unregister`：反注册事件（UObject）
* `FGES::RegisterRaw`：注册事件（非UObject）
* `FGES::UnregisterRaw`：反注册事件（非UObject）
* `FGES::Dispatch`：触发事件
## 用法
* Bool
``` c++
FGES::Register(TEXT("BoolTestEvent"), this, &AGESTest::OnBoolEvent);
FGES::Unregister(TEXT("BoolTestEvent"), this, &AGESTest::OnBoolEvent);
FGES::Dispatch("BoolTestEvent", true);

void AGESTest::OnBoolEvent(const FGESEventDataArray& EventData)
{
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
    const FText& TextValue = EventData.GetText(0);
}
```
* Enum
``` c++
FGES::Register(TEXT("EnumTestEvent"), this, &AGESTest::OnEnumEvent);
FGES::Unregister(TEXT("EnumTestEvent"), this, &AGESTest::OnEnumEvent);

// 注：为了兼容lua，枚举类型在传输时需要手动强转为int类型
FGES::Dispatch("EnumTestEvent", (int32)EGESCppType::UEnum);

void AGESTest::OnEnumEvent(const FGESEventDataArray& EventData)
{
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
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
    // EventData数据下标从0开始
    const TMap<int32, FVector> MapValue = EventData.GetMap<int32, FVector>(0);
}
```
## 注意事项
* Dispatch传参说明
> 考虑性能，FGES::Dispatch在传参时，除了基础类型或立即数以外，其余类型不支持右值作为实参。  
> 例如：  
> `FGES::Dispatch("BoolTestEvent", true);`=============><font color=green>**可以**</font>  
> `FGES::Dispatch("StructTestEvent", FVector(1, 2, 3));`=><font color=red>**不可以**</font>
>
> 如果要支持这个功能，需要在底层存储数据时做内存拷贝，这会浪费一部分内存，因此考虑到性能，也不希望这么做。另外通常游戏开发中事件系统在传参数时很少需要直接用右值，如果参数确实是右值，用一个临时变量(左值)代替即可：  
> `FVector VectorValue = GetVector();`  
> `FGES::Dispatch("StructTestEvent", VectorValue);`=====><font color=green>**可以**</font>
* 枚举类型
```c++
// 注：为了兼容lua，枚举类型在传输时需要手动强转为int类型
FGES::Dispatch("EnumTestEvent", (int32)EGESCppType::UEnum);
```

> 注：更多详情参见 [GESTest.cpp](../../GESProj/Source/GESProj/Test/GESTest.cpp)