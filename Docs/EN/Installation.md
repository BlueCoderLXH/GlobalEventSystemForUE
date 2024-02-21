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
> 注：更多详情参见 [GESGameInstance.cpp](../../GESProj/Source/GESProj/GESGameInstance.cpp)