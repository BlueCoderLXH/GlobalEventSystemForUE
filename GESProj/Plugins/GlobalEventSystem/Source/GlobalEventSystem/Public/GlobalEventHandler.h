#pragma once

#include <functional>

#include "GlobalEventData.h"

DEFINE_LOG_CATEGORY_STATIC(LogGlobalEventSystem, Log, All);

DECLARE_MULTICAST_DELEGATE_OneParam(FGlobalEventListener, const FGlobalEventData&);

typedef void (*FGlobalEventStaticDelegate)(const FGlobalEventData&);

struct FGlobalEventCallbackKey
{
    FGlobalEventCallbackKey(void* InCallbackPtr) {
        HashCode = HashCombine(PointerHash(nullptr), PointerHash(InCallbackPtr));
    }

    FGlobalEventCallbackKey(void* InTarget, void* InCallbackPtr) {
        HashCode = HashCombine(PointerHash(InTarget), PointerHash(InCallbackPtr));
    }

    bool operator==(const FGlobalEventCallbackKey& Other) const {
        return HashCode == Other.HashCode;
    }

    uint32 GetHashCode() const {
        return HashCode;
    }

private:
    uint32 HashCode;
};

inline uint32 GetTypeHash(const FGlobalEventCallbackKey& Key) {
    return Key.GetHashCode();
}

/**
 * FGameEventListeners
 * The collection of FGameEventListener
 */
class GLOBALEVENTSYSTEM_API FGlobalEventListeners
{
public:
    void Init(const FGlobalEventType& InEventID) {
        EventID = InEventID;
    }

    bool Register(const FGlobalEventStaticDelegate& InCallback) {
        auto BindListener = [&] () -> FDelegateHandle {
            return GetListener().AddStatic(InCallback);
        };

        return RegisterInner(nullptr, PointerCast<void*>(InCallback), BindListener);
    }

    bool Unregister(const FGlobalEventStaticDelegate& InCallback) {
        auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
            GetListener().Remove(DelegateHandle);
        };

        return UnregisterInner(nullptr, PointerCast<void*>(InCallback), UnBindListener);
    }

    template<typename ClassType>
    bool Register(
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGlobalEventData&)) {
        auto BindListener = [&] () -> FDelegateHandle {
            return GetListener(InTarget).AddUObject(InTarget, InCallback);
        };

        return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);
    }

    template<typename ClassType>
    bool Unregister(
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGlobalEventData&)) {
        auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
            GetListener(InTarget).Remove(DelegateHandle);
        };

        return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);
    }

    template<typename ClassType>
    bool RegisterRaw(ClassType* InTarget, void (ClassType::* InCallback) (const FGlobalEventData&)) {
        auto BindListener = [&] () -> FDelegateHandle {
            return GetListener().AddRaw(InTarget, InCallback);
        };

        return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);
    }

    template<typename ClassType>
    bool UnregisterRaw(ClassType* InTarget, void (ClassType::* InCallback) (const FGlobalEventData&)) {
        auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
            GetListener().Remove(DelegateHandle);
        };

        return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);
    }

    bool Dispatch(const FGlobalEventData& EventData);

    void Clear();

private:
    bool RegisterInner(void* InTarget, void* InCallback, std::function<FDelegateHandle()> BindListener) {
        const FGlobalEventCallbackKey FuncKey(InTarget, InCallback);
        if (HandleMap.Find(FuncKey)) {
            UE_LOG(LogGlobalEventSystem, Warning, TEXT("Register an existent callback for event:%s"), *EventID.ToString());
            return false;
        }

        const auto DelegateHandle = BindListener();
        HandleMap.Add(FuncKey, DelegateHandle);
        return true;
    }

    bool UnregisterInner(void* InTarget, void* InCallback, std::function<void(const FDelegateHandle&)> UnBindListener) {
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

    FGlobalEventListener& GetListener(const UObject* WorldContext = nullptr) {
        return Listeners;
    }

    template<typename DstType, typename SrcType>
    static DstType PointerCast(SrcType SrcPtr) {
        return *static_cast<DstType*>(static_cast<void*>(&SrcPtr));
    }

    FGlobalEventType EventID;

    FGlobalEventListener Listeners;

    TMap<FGlobalEventCallbackKey, FDelegateHandle> HandleMap;
};

/**
 * FGameEventHandler
 * Handler the event action:
 * - Register()
 * - Unregister()
 * - Dispatch()
 */
class GLOBALEVENTSYSTEM_API FGlobalEventHandler
{
public:
    bool Register(
        const FGlobalEventType& InEventID,
        const FGlobalEventStaticDelegate InStaticCallback);

    bool Unregister(
        const FGlobalEventType& InEventID,
        const FGlobalEventStaticDelegate InStaticCallback);

    template<typename ClassType>
    bool Register(
        const FGlobalEventType& InEventID,
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGlobalEventData&)) {
        
        FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
        ListenersPtr.Init(InEventID);

        if (!ListenersPtr.Register(InTarget, InCallback)) {
            return false;
        }
        
        return true;
    }

    template<typename ClassType>
    bool Unregister(
        const FGlobalEventType& InEventID,
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGlobalEventData&)) {
        
        FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
        if (!ListenersPtr) {
            return false;
        }

        return ListenersPtr->Unregister(InTarget, InCallback);
    }

    template<typename ClassType>
    bool RegisterRaw(
        const FGlobalEventType& InEventID, ClassType* InTarget,
        void (ClassType::* InCallback) (const FGlobalEventData&)) {
        
        FGlobalEventListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
        ListenersPtr.Init(InEventID);

        if (!ListenersPtr.RegisterRaw(InTarget, InCallback)) {
            return false;
        }
        
        return true;
    }

    template<typename ClassType>
    bool UnregisterRaw(
        const FGlobalEventType& InEventID, ClassType* InTarget,
        void (ClassType::* InCallback) (const FGlobalEventData&)) {

        FGlobalEventListeners* ListenersPtr = EventMap.Find(InEventID);
        if (!ListenersPtr) {
            return false;
        }

        return ListenersPtr->UnregisterRaw(InTarget, InCallback);
    }

    bool Dispatch(const FGlobalEventData& EventData);

    void Clear();

private:
    TMap<FGlobalEventType, FGlobalEventListeners> EventMap;
};
