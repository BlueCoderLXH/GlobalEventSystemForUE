# 安装GES
#### Plugin
将以下拷贝到本地项目的`Plugins`目录
* `GESProj\Plugins\GlobalEventSystem`
* `GESProj\Plugins\UnLua`
#### Lua
将以下拷贝到本地项目的`Content\Script`目录
* `GESProj\Content\Script`
> 注：`GESProj\Content\Script\Test`可以忽略
#### C++
* ***.Build.cs设置**  
为了在C++中使用GES，需要在对应*.Build.cs中添加对GES的Module引用
```csharp
PrivateDependencyModuleNames.AddRange(new string[]
{
    "GES",
}
```
> 范例参见：[GESProj.Build.cs](../../GESProj/Source/GESProj/GESProj.Build.cs)
* **Init/Shutdown**  
游戏启动和结束时，需要对GES分别进行Init和Shutdown。  
一个推荐的时机就是在GameInstance的Init和Shutdown执行：  
```c++
void UGESGameInstance::Init()
{
	Super::Init();

	// Initialize GES
	FGES::Init();
}

void UGESGameInstance::Shutdown()
{
	// Shutdown GES
	FGES::Shutdown();
	
	Super::Shutdown();
}
```
> 范例参见：[GESGameInstance.cpp](../../GESProj/Source/GESProj/GESGameInstance.cpp)