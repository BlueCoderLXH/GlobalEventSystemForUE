#include "GESHandler.h"

//---------- FGESListeners ----------
bool FGESListeners::RegisterStatic(const FGESStaticDelegate& InCallback)
{
	auto BindListener = [&]() -> FDelegateHandle
	{
		return GetListener().AddStatic(InCallback);
	};

	return RegisterInner(nullptr, PointerCast<void*>(InCallback), BindListener);
}

bool FGESListeners::UnregisterStatic(const FGESStaticDelegate& InCallback)
{
	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener().Remove(DelegateHandle);
	};

	return UnregisterInner(nullptr, PointerCast<void*>(InCallback), UnBindListener);
}

template <typename ClassType>
bool FGESListeners::Register(
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto BindListener = [&] () -> FDelegateHandle {
		return GetListener(InTarget).AddUObject(InTarget, InCallback);
	};

	return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);
}

template <typename ClassType>
bool FGESListeners::Unregister(
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener(InTarget).Remove(DelegateHandle);
	};

	return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);
}

template <typename ClassType>
bool FGESListeners::RegisterRaw(ClassType* InTarget, void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto BindListener = [&] () -> FDelegateHandle {
		return GetListener().AddRaw(InTarget, InCallback);
	};

	return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);
}

template <typename ClassType>
bool FGESListeners::UnregisterRaw(ClassType* InTarget, void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener().Remove(DelegateHandle);
	};

	return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);
}

bool FGESListeners::RegisterBP(UObject* InTarget, const FName InFunctionName)
{
	const UFunction* BPFunction = InTarget->FindFunctionChecked(InFunctionName);
        
	auto BindListener = [&] () -> FDelegateHandle {
		return GetListener(InTarget).AddUFunction(InTarget, InFunctionName);
	};

	return RegisterInner(InTarget, PointerCast<void*>(BPFunction->GetNativeFunc()), BindListener);
}

bool FGESListeners::UnregisterBP(UObject* InTarget, const FName InFunctionName)
{
	const UFunction* BPFunction = InTarget->FindFunctionChecked(InFunctionName);

	auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
		GetListener().Remove(DelegateHandle);
	};

	return UnregisterInner(InTarget, PointerCast<void*>(BPFunction->GetNativeFunc()), UnBindListener);
}

bool FGESListeners::RegisterInner(void* InTarget, void* InCallback, std::function<FDelegateHandle()> BindListener)
{
	const FGESCallbackKey FuncKey(InTarget, InCallback);
	if (HandleMap.Find(FuncKey)) {
		UE_LOG(LogGES, Warning, TEXT("Register an existent callback for event:%s"), *EventID.ToString());
		return false;
	}

	const auto DelegateHandle = BindListener();
	HandleMap.Add(FuncKey, DelegateHandle);
	return true;
}

bool FGESListeners::UnregisterInner(void* InTarget, void* InCallback, std::function<void(const FDelegateHandle&)> UnBindListener)
{
	const FGESCallbackKey FuncKey(InTarget, InCallback);
	if (!HandleMap.Find(FuncKey)) {
		UE_LOG(LogGES, Warning, TEXT("Unregister an inexistent callback for event:%s"), *EventID.ToString());
		return false;
	}

	const auto DelegateHandle = HandleMap[FuncKey];
	UnBindListener(DelegateHandle);
	HandleMap.Remove(FuncKey);
	return true;
}

bool FGESListeners::Dispatch(const FGESEventDataArray& EventData)
{
	if (!GetListener().IsBound())
	{
		return false;
	}

	GetListener().Broadcast(EventData);
	return true;
}

void FGESListeners::Clear()
{
	Listeners.Clear();
	HandleMap.Empty();
}

//---------- FGESHandler ----------
bool FGESHandler::RegisterStatic(
	const FGESEventType& InEventID,
	const FGESStaticDelegate InStaticCallback)
{
	FGESListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.RegisterStatic(InStaticCallback))
	{
		return false;
	}

	return true;
}

bool FGESHandler::UnregisterStatic(
	const FGESEventType& InEventID,
	const FGESStaticDelegate InStaticCallback)
{
	FGESListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr)
	{
		return false;
	}

	return ListenersPtr->UnregisterStatic(InStaticCallback);
}

template <typename ClassType>
bool FGESHandler::Register(
	const FGESEventType& InEventID,
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
        
	FGESListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.Register(InTarget, InCallback)) {
		return false;
	}
        
	return true;
}

template <typename ClassType>
bool FGESHandler::Unregister(
	const FGESEventType& InEventID,
	std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	FGESListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr) {
		return false;
	}

	return ListenersPtr->Unregister(InTarget, InCallback);
}

template <typename ClassType>
bool FGESHandler::RegisterRaw(
	const FGESEventType& InEventID, ClassType* InTarget,
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	FGESListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.RegisterRaw(InTarget, InCallback)) {
		return false;
	}
        
	return true;
}

template <typename ClassType>
bool FGESHandler::UnregisterRaw(
	const FGESEventType& InEventID,
	ClassType* InTarget, 
	void (ClassType::*InCallback)(const FGESEventDataArray&))
{
	FGESListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr) {
		return false;
	}

	return ListenersPtr->UnregisterRaw(InTarget, InCallback);
}

bool FGESHandler::RegisterBP(const FGESEventType& InEventID, const UObject* InTarget, const FName InFunctionName)
{
	FGESListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
	ListenersPtr.Init(InEventID);

	if (!ListenersPtr.RegisterBP(const_cast<UObject*>(InTarget), InFunctionName))
	{
		return false;
	}

	return true;
}

bool FGESHandler::UnregisterBP(const FGESEventType& InEventID, const UObject* InTarget, const FName InFunctionName)
{
	FGESListeners* ListenersPtr = EventMap.Find(InEventID);
	if (!ListenersPtr) {
		return false;
	}

	return ListenersPtr->UnregisterBP(const_cast<UObject*>(InTarget), InFunctionName);
}


bool FGESHandler::Dispatch(const FGESEventDataArray& EventData)
{
	const auto ListenersPtr = EventMap.Find(EventData.GetEventID());
	if (!ListenersPtr)
	{
		return false;
	}

	return ListenersPtr->Dispatch(EventData);
}

void FGESHandler::Clear()
{
	for (auto Listeners : EventMap)
	{
		Listeners.Value.Clear();
	}

	EventMap.Empty();
}
