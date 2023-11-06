#include "..\Public\GlobalEventHandler.h"

//---------- FGameEventListeners ----------
bool FGlobalEventListeners::Dispatch(const FGlobalEventData& EventData) {
    if (!GetListener().IsBound()) {
        return false;
    }

    GetListener().Broadcast(EventData);
    return true;
}

void FGlobalEventListeners::Clear() {
    Listeners.Clear();
    HandleMap.Empty();
}

//---------- FGameEventHandler ----------
bool FGlobalEventHandler::Register(
    const FGlobalEventType& InEventID,
    const FGlobalEventStaticDelegate InStaticCallback) {
    
    FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
    ListenersPtr.Init(InEventID);

    if (!ListenersPtr.Register(InStaticCallback)) {
        return false;
    }
    
    return true;
}

bool FGlobalEventHandler::Unregister(
    const FGlobalEventType& InEventID,
    const FGlobalEventStaticDelegate InStaticCallback) {
    FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
    if (!ListenersPtr) {
        return false;
    }

    return ListenersPtr->Unregister(InStaticCallback);
}

bool FGlobalEventHandler::Dispatch(const FGlobalEventData& EventData) {
    const auto ListenersPtr = EventMap.Find(EventData.EventID);
    if (!ListenersPtr) {
        return false;
    }

    return ListenersPtr->Dispatch(EventData);
}

void FGlobalEventHandler::Clear() {
    for (auto Listeners : EventMap) {
        Listeners.Value.Clear();
    }

    EventMap.Empty();
}
