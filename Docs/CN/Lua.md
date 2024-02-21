# Lua
## API
```csharp
--- Global event system for lua
local GlobalEventSystem = {
    --- GES Event Data Type
    Bool    = "Bool",
    Integer = "Integer",
    Float   = "Float",
    String  = "FString",
    Name    = "FName",
    Text    = "FText",
    Enum    = "Enum",
    Struct  = "UStruct",
    Object  = "UObject",
    Class   = "UClass",
    Array   = "TArray",
    Map     = "TMap",
    Set     = "TSet",
    --- GES Event Data Type
}
GES = GlobalEventSystem
```
* 注册事件  
```csharp
--- Register global event for lua
--- @param table CppEvent Cpp event, defined in 'CppEvents.lua'
--- @param delegate Callback Lua Callback for this event, eg. {self, self.CallbackName}
function GlobalEventSystem.Register(CppEvent, Callback)
```
* 反注册事件
```csharp
--- Unregister global event for lua
--- @param table CppEvent Cpp event, defined in 'CppEvents.lua'
--- @param delegate Callback Lua Callback for this event, eg. {self, self.CallbackName}
function GlobalEventSystem.Unregister(CppEvent, Callback)
```
* 触发事件
```csharp
--- Dispatch global event from lua
--- @param table CppEvent Cpp event, defined in 'CppEvents.lua'
--- @param args ... Event params, eg. { ParamTypeName, Param }
function GlobalEventSystem.Dispatch(CppEvent, ...)
```
> 更多详情参见lua源码：[GlobalEventSystem.lua](../../GESProj/Content/Script/GlobalEventSystem/GlobalEventSystem.lua)
## 用法
### 注册事件
```csharp
GES.Register(CppEvents.BoolTestEvent, { self, self.OnBoolTestEvent })
```
### 反注册事件
```csharp
GES.Unregister(CppEvents.BoolTestEvent, { self, self.OnBoolTestEvent })
```
### 触发事件
```csharp
GES.Dispatch(CppEvents.BoolTestEvent, { GES.Bool, true })
```
> 更多详情参见lua源码：[BP_GESTest.lua](../../GESProj/Content/Script/Test/BP_GESTest.lua)