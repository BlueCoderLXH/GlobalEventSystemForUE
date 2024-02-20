# Event Data Config
### Config Path
[CppEvents.lua](../../GESProj/Content/Script/GlobalEventSystem/CppEvents.lua) `GESProj/Content/Script/GlobalEventSystem/CppEvents.lua`
### Config Item
```csharp
--- GES Container Type
local CT = UE4.EGESContainerType
--- GES Event Data Cpp Type
local EDC = UE4.EGESCppType

---
---CppEvents Definition
---
--local EventTable = {
--    Name="",    -- Event name
--    Data= {     -- Event data array
--        [1] = {
--            ContainerType = CT.None,      -- Container type, see cpp type 'EGESContainerType'
--            CppType = EDC.None,           -- Cpp type, see cpp type 'EGESCppType'
--            CppSubTypeName = "",          -- Cpp sub type name, UObject:UClass name, UStruct: Struct name without prefix 'F', UEnum: Enum name must with prefix 'E'
--            ValueType = EDC.None,         -- Only when ContainerType is TMap<Key, Value>, CppType & CppSubTypeName is used for 'Key', ValueType & ValueSubTypeName is used for 'Value'
--            ValueSubTypeName = "",        -- TMap Value sub type
--        },
--        -- Add more if necessary
--        [2] = {
--            ...
--        },
--        [3] = {
--            ...
--        }
--        ...
--    }
--}
```
### Support Hot Reload In Editor
Input console command `ges.hotreload`：  
* `ges.hotreload true`: Open config's hot reload in editor，it will load lua config every time  
* `ges.hotreload false`: Close config's hot reload in editor
> Usually used for blueprint in editor，it should be closed in runtime，or it will affect performance