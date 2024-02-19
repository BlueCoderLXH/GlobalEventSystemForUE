#include "GESHandler.h"

//---------- FGESListeners ----------
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
