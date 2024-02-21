# GlobalEventSystemForUE
[![GitHub release](https://img.shields.io/github/v/release/BlueCoderLXH/GlobalEventSystemForUE)](https://github.com/BlueCoderLXH/GlobalEventSystemForUE/releases)
[![GitHub all downloads](https://img.shields.io/github/downloads/BlueCoderLXH/GlobalEventSystemForUE/total?color=25c2a0)](https://github.com/BlueCoderLXH/GlobalEventSystemForUE/releases)
## Overview
GES(GlobalEventSystem)，a global event system developed based on UE engine 4.26，  
Features：
* **Event Data Interworking**  
  Event data can be transferred between **C++**，**Lua** and **Blueprint**
* **Blueprint Support**  
  ![RegisterEvent](Docs/Images/BP_GESRegister1.png "GES Register Event")  
  ![UnregisterEvent](Docs/Images/BP_GESUnregister1.png "GES Unregister Event")  
  ![DispatchEvent](Docs/Images/BP_GESDispatch1.png "GES Dispatch Event")
> See more：[Blueprint](Docs/EN/Blueprint.md)
* **Event Config Support Hot Reload**
* **Tiny**
* **Extendable**
* **Easily usable**
## Engine Support
Up to now, support`UE4.26` and `UE5.3`
> * The older version of UE4.26 isn't guaranteed  
> * UE4.26+ is supported theoretically  
> * The older version of UE5.3 may not be supported, because older version use C++17, but UE5.3 use C++20
## Install
[Install GES to local project](Docs/EN/Installation.md)
## How to use GES
* [Event Data Config](Docs/EN/CppEventConfig.md)
* [C++](Docs/EN/C++.md)
* [Blueprint](Docs/EN/Blueprint.md)
* [Lua](Docs/EN/Lua.md)

> If any question, contact me：  
> **My Blog**&nbsp;：[bluecoder](https://www.zhihu.com/people/bluecoder_lxh)  
> **My Email**：bluecoder@yeah.net