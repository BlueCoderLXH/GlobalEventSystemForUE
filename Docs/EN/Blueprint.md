# Blueprint
## API
* Register Event  
`GESRegisterEvent`  
![RegisterEvent](../Images/BP_GESRegister.png "GES Register Event")
* Unregister Event  
`GESUnregisterEvent`  
![UnregisterEvent](../Images/BP_GESUnregister.png "GES Unregister Event")
* Dispatch Event  
`GESDispatchEvent`  
![DispatchEvent](../Images/BP_GESDispatch.png "GES Dispatch Event")
## Usage
### Register Event
Fill in `EventType`，then it will recognize event ID and create event data pins match the type automatically  

eg：  
![RegisterEvent](../Images/BP_GESRegister1.png "GES Register Event")
### Unregister Event
Fill in `EventType`，then is will unbind callback for this event in current blueprint automatically  
![UnregisterEvent](../Images/BP_GESUnregister1.png "GES Unregister Event")
### Dispatch Event
Same as `GESRegisterEvent`  
![DispatchEvent](../Images/BP_GESDispatch1.png "GES Dispatch Event")  

> See more：[BP_GESTest.uasset](../../GESProj/Content/Blueprints/BP_GESTest.uasset)