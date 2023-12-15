#include "..\Public\GlobalEventHandler.h"

//---------- FGameEventListeners ----------
bool FGlobalEventListeners::Register(const FGlobalEventStaticDelegate& InCallback)
{
	auto BindListener = [&]() -> FDelegateHandle
	{
		return GetListener().AddStatic(InCallback);
	};

	return RegisterInner(nullptr, PointerCast<void*>(InCallback), BindListener);
}

bool FGlobalEventListeners::Unregister(const FGlobalEventStaticDelegate& InCallback)
{
	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener().Remove(DelegateHandle);
	};

	return UnregisterInner(nullptr, PointerCast<void*>(InCallback), UnBindListener);
}

template <typename ClassType>
bool FGlobalEventListeners::Register(
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto BindListener = [&] () -> FDelegateHandle {
		return GetListener(InTarget).AddUObject(InTarget, InCallback);
	};

	return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);
}

template <typename ClassType>
bool FGlobalEventListeners::Unregister(
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener(InTarget).Remove(DelegateHandle);
	};

	return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);
}

template <typename ClassType>
bool FGlobalEventListeners::RegisterRaw(ClassType* InTarget, void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto BindListener = [&] () -> FDelegateHandle {
		return GetListener().AddRaw(InTarget, InCallback);
	};

	return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);
}

template <typename ClassType>
bool FGlobalEventListeners::UnregisterRaw(ClassType* InTarget, void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener().Remove(DelegateHandle);
	};

	return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);
}

bool FGlobalEventListeners::RegisterBP(UObject* InTarget, const FName InFunctionName)
{
	const UFunction* BPFunction = InTarget->FindFunctionChecked(InFunctionName);
        
	auto BindListener = [&] () -> FDelegateHandle {
		return GetListener(InTarget).AddUFunction(InTarget, InFunctionName);
	};

	return RegisterInner(InTarget, PointerCast<void*>(BPFunction->GetNativeFunc()), BindListener);
}

bool FGlobalEventListeners::UnregisterBP(UObject* InTarget, const FName InFunctionName)
{
	const UFunction* BPFunction = InTarget->FindFunctionChecked(InFunctionName);

	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener().Remove(DelegateHandle);
	};

	return UnregisterInner(InTarget, PointerCast<void*>(BPFunction->GetNativeFunc()), UnBindListener);
}

bool FGlobalEventListeners::RegisterInner(void* InTarget, void* InCallback, std::function<FDelegateHandle()> BindListener)
{
	const FGlobalEventCallbackKey FuncKey(InTarget, InCallback);
	if (HandleMap.Find(FuncKey)) {
		UE_LOG(LogGlobalEventSystem, Warning, TEXT("Register an existent callback for event:%s"), *EventID.ToString());
		return false;
	}

	const auto DelegateHandle = BindListener();
	HandleMap.Add(FuncKey, DelegateHandle);
	return true;
}

bool FGlobalEventListeners::UnregisterInner(void* InTarget, void* InCallback, std::function<void(const FDelegateHandle&)> UnBindListener)
{
	const FGlobalEventCallbackKey FuncKey(InTarget, InCallback);
	if (!HandleMap.Find(FuncKey)) {
		UE_LOG(LogGlobalEventSystem, Warning, TEXT("Unregister an inexistent callback for event:%s"), *EventID.ToString());
		return false;
	}

	const auto DelegateHandle = HandleMap[FuncKey];
	UnBindListener(DelegateHandle);
	HandleMap.Remove(FuncKey);
	return true;
}

bool FGlobalEventListeners::Dispatch(const FGESEventDataArray& EventData)
{
	if (!GetListener().IsBound())
	{
		return false;
	}

	GetListener().Broadcast(EventData);
	return true;
}

void FGlobalEventListeners::Clear()
{
	Listeners.Clear();
	HandleMap.Empty();
}

//---------- FGameEventHandler ----------
bool FGlobalEventHandler::Register(
	const FGESEventType& InEventID,
	const FGlobalEventStaticDelegate InStaticCallback)
{
	FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.Register(InStaticCallback))
	{
		return false;
	}

	return true;
}

bool FGlobalEventHandler::Unregister(
	const FGESEventType& InEventID,
	const FGlobalEventStaticDelegate InStaticCallback)
{
	FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr)
	{
		return false;
	}

	return ListenersPtr->Unregister(InStaticCallback);
}

template <typename ClassType>
bool FGlobalEventHandler::Register(
	const FGESEventType& InEventID,
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
        
	FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.Register(InTarget, InCallback)) {
		return false;
	}
        
	return true;
}

template <typename ClassType>
bool FGlobalEventHandler::Unregister(
	const FGESEventType& InEventID,
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr) {
		return false;
	}

	return ListenersPtr->Unregister(InTarget, InCallback);
}

template <typename ClassType>
bool FGlobalEventHandler::RegisterRaw(
	const FGESEventType& InEventID, ClassType* InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.RegisterRaw(InTarget, InCallback)) {
		return false;
	}
        
	return true;
}

template <typename ClassType>
bool FGlobalEventHandler::UnregisterRaw(
	const FGESEventType& InEventID,
	ClassType* InTarget, 
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr) {
		return false;
	}

	return ListenersPtr->UnregisterRaw(InTarget, InCallback);
}

bool FGlobalEventHandler::RegisterBP(const FGESEventType& InEventID, const UObject* InTarget, const FName InFunctionName)
{
	FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.RegisterBP(const_cast<UObject*>(InTarget), InFunctionName))
	{
		return false;
	}

	return true;
}

bool FGlobalEventHandler::UnregisterBP(const FGESEventType& InEventID, const UObject* InTarget, const FName InFunctionName)
{
	FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr) {
		return false;
	}

	return ListenersPtr->UnregisterBP(const_cast<UObject*>(InTarget), InFunctionName);
}


bool FGlobalEventHandler::Dispatch(const FGESEventDataArray& EventData)
{
	const auto ListenersPtr = EventMap.Find(EventData.GetEventID());
	if (!ListenersPtr)
	{
		return false;
	}

	return ListenersPtr->Dispatch(EventData);
}

void FGlobalEventHandler::Clear()
{
	for (auto Listeners : EventMap)
	{
		Listeners.Value.Clear();
	}

	EventMap.Empty();
}
