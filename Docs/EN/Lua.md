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
* Register Event  
```csharp
--- Register global event for lua
--- @param table CppEvent Cpp event, defined in 'CppEvents.lua'
--- @param delegate Callback Lua Callback for this event, eg. {self, self.CallbackName}
function GlobalEventSystem.Register(CppEvent, Callback)
```
* Unregister Event
```csharp
--- Unregister global event for lua
--- @param table CppEvent Cpp event, defined in 'CppEvents.lua'
--- @param delegate Callback Lua Callback for this event, eg. {self, self.CallbackName}
function GlobalEventSystem.Unregister(CppEvent, Callback)
```
* Dispatch Event
```csharp
--- Dispatch global event from lua
--- @param table CppEvent Cpp event, defined in 'CppEvents.lua'
--- @param args ... Event params, eg. { ParamTypeName, Param }
function GlobalEventSystem.Dispatch(CppEvent, ...)
```
> See more [GlobalEventSystem.lua](../../GESProj/Content/Script/GlobalEventSystem/GlobalEventSystem.lua)
## Usage
### Register Event
```csharp
GES.Register(CppEvents.BoolTestEvent, { self, self.OnBoolTestEvent })
```
### Unregister Event
```csharp
GES.Unregister(CppEvents.BoolTestEvent, { self, self.OnBoolTestEvent })
```
### Dispatch Event
```csharp
GES.Dispatch(CppEvents.BoolTestEvent, { GES.Bool, true })
```
> See more [BP_GESTest.lua](../../GESProj/Content/Script/Test/BP_GESTest.lua)