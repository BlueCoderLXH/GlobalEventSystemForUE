# 事件数据配置
### 配置路径
[CppEvents.lua](../../GESProj/Content/Script/GlobalEventSystem/CppEvents.lua) `GESProj/Content/Script/GlobalEventSystem/CppEvents.lua`
### 配置项说明
```text
--- 容器类型
local CT = UE4.EGESContainerType
--- 事件数据C++类型
local EDC = UE4.EGESCppType

---
---CppEvents Definition
---
--local EventTable = {
--    Name="",    -- 事件ID名
--    Data= {     -- 事件数据类型定义
--        [1] = {
--            ContainerType = CT.None,      -- 容器类型, 具体参见C++枚举'EGESContainerType'
--            CppType = EDC.None,           -- C++数据类型, 具体参见C++枚举'EGESCppType'
--            CppSubTypeName = "",          -- C++数据子类型, UObject:UClass名, UStruct: Struct名不带前缀'F', UEnum: Enum名必须带前缀'E'
--            ValueType = EDC.None,         -- 当容器类型为TMap<Key, Value>时, CppType & CppSubTypeName用于定义'Key', ValueType & ValueSubTypeName用于定义'Value'
--            ValueSubTypeName = "",        -- TMap Value子类型(规范同CppSubTypeName)
--        },
--        -- 事件数据多个参数, 以此类推
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
### Editor模式下支持配置热加载
console中输入命令 `ges.hotreload`，其中：  
* `ges.hotreload true`: Editor模式下打开配置热加载，每次读取配置数据时都会重新读取lua配置，以使配置数据实时更新起效  
* `ges.hotreload false`: Editor模式下关闭配置热加载
> 主要用于Editor模式下蓝图编辑，运行时建议关闭，否则会有一定程度的性能影响