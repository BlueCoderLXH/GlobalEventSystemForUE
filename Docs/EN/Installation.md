# Install GES
#### Plugin
Copy the followings to local project directory `Plugins`  
* `GESProj\Plugins\GlobalEventSystem`
* `GESProj\Plugins\UnLua`
#### Lua
Copy the followings to local project directory `Content\Script`  
* `GESProj\Content\Script`
> Attention：`GESProj\Content\Script\Test` can be ignored
#### C++
* ***.Build.cs Setting**  
  To use `GES` in C++，we need to add module dependency in *.Build.cs file 
```csharp
PrivateDependencyModuleNames.AddRange(new string[]
{
    "GES",
}
```
> See more [GESProj.Build.cs](../../GESProj/Source/GESProj/GESProj.Build.cs)
* **Init/Shutdown**  
GES need to `Init` and `Shutdown` when game start and shutdown.  
The `Init` and `Shutdown` of `GameInstance` is a recommended chance:  
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
> See more：[GESGameInstance.cpp](../../GESProj/Source/GESProj/GESGameInstance.cpp)