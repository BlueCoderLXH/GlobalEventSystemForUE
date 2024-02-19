#pragma once

#include <functional>

#include "GESEventData.h"

DEFINE_LOG_CATEGORY_STATIC(LogGES, Log, All);

DECLARE_MULTICAST_DELEGATE_OneParam(FGESListener, const FGESEventDataArray&);

typedef void (*FGESStaticDelegate)(const FGESEventDataArray&);

struct FGESCallbackKey
{
    FGESCallbackKey(const void* InCallbackPtr) {
        HashCode = HashCombine(PointerHash(nullptr), PointerHash(InCallbackPtr));
    }

    FGESCallbackKey(const void* InTarget, const void* InCallbackPtr) {
        HashCode = HashCombine(PointerHash(InTarget), PointerHash(InCallbackPtr));
    }

    bool operator==(const FGESCallbackKey& Other) const {
        return HashCode == Other.HashCode;
    }

    uint32 GetHashCode() const {
        return HashCode;
    }

private:
    uint32 HashCode;
};

inline uint32 GetTypeHash(const FGESCallbackKey& Key) {
    return Key.GetHashCode();
}

/**
 * FGESListeners
 * The collection of FGESListener
 */
class GES_API FGESListeners
{
private:
    FGESEventType EventID;

    FGESListener Listeners;

    TMap<FGESCallbackKey, FDelegateHandle> HandleMap;
    
public:
    void Init(const FGESEventType& InEventID) {
        EventID = InEventID;
    }

    bool RegisterStatic(const FGESStaticDelegate& InCallback)
    {
        auto BindListener = [&]() -> FDelegateHandle
        {
            return GetListener().AddStatic(InCallback);
        };

        return RegisterInner(nullptr, PointerCast<void*>(InCallback), BindListener);        
    }
    
    bool UnregisterStatic(const FGESStaticDelegate& InCallback)
    {
        auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
            GetListener().Remove(DelegateHandle);
        };

        return UnregisterInner(nullptr, PointerCast<void*>(InCallback), UnBindListener);        
    }

    template<typename ClassType>
    bool Register(
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        auto BindListener = [&] () -> FDelegateHandle {
            return GetListener(InTarget).AddUObject(InTarget, InCallback);
        };

        return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);        
    }
    
    template<typename ClassType>
    bool Unregister(
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
            GetListener(InTarget).Remove(DelegateHandle);
        };

        return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);        
    }

    template<typename ClassType>
    bool RegisterRaw(ClassType* InTarget, void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        auto BindListener = [&] () -> FDelegateHandle {
            return GetListener().AddRaw(InTarget, InCallback);
        };

        return RegisterInner(InTarget, PointerCast<void*>(InCallback), BindListener);        
    }
    
    template<typename ClassType>
    bool UnregisterRaw(ClassType* InTarget, void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        auto UnBindListener = [&] (const FDelegateHandle& DelegateHandle) -> void {
            GetListener().Remove(DelegateHandle);
        };

        return UnregisterInner(InTarget, PointerCast<void*>(InCallback), UnBindListener);        
    }
    
    bool RegisterBP(UObject* InTarget, const FName InFunctionName);
    bool UnregisterBP(UObject* InTarget, const FName InFunctionName);

    bool Dispatch(const FGESEventDataArray& EventData);

    void Clear();

private:
    bool RegisterInner(void* InTarget, void* InCallback, std::function<FDelegateHandle()> BindListener);
    bool UnregisterInner(void* InTarget, void* InCallback, std::function<void(const FDelegateHandle&)> UnBindListener);

    FGESListener& GetListener(const UObject* WorldContext = nullptr) { return Listeners; }

    template<typename DstType, typename SrcType>
    static DstType PointerCast(SrcType SrcPtr) {
        return *static_cast<DstType*>(static_cast<void*>(&SrcPtr));
    }
};

/**
 * FGESHandler
 * Handle the event action:
 * - Register()
 * - Unregister()
 * - Dispatch()
 */
class GES_API FGESHandler
{
public:
    bool RegisterStatic(
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
    
    bool UnregisterStatic(
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

    template<typename ClassType>
    bool Register(
        const FGESEventType& InEventID,
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        
        FGESListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
        ListenersPtr.Init(InEventID);

        if (!ListenersPtr.Register(InTarget, InCallback)) {
            return false;
        }
        
        return true;
    }
    
    template<typename ClassType>
    bool Unregister(
        const FGESEventType& InEventID,
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        FGESListeners* ListenersPtr = EventMap.Find(InEventID);
        if (!ListenersPtr) {
            return false;
        }

        return ListenersPtr->Unregister(InTarget, InCallback);
    }

    template<typename ClassType>
    bool RegisterRaw(
        const FGESEventType& InEventID, ClassType* InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        FGESListeners& ListenersPtr = EventMap.FindOrAdd(InEventID);
        ListenersPtr.Init(InEventID);

        if (!ListenersPtr.RegisterRaw(InTarget, InCallback)) {
            return false;
        }
        
        return true;        
    }
    
    template<typename ClassType>
    bool UnregisterRaw(
        const FGESEventType& InEventID, ClassType* InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&))
    {
        FGESListeners* ListenersPtr = EventMap.Find(InEventID);
        if (!ListenersPtr) {
            return false;
        }

        return ListenersPtr->UnregisterRaw(InTarget, InCallback);
    }

    bool RegisterBP(const FGESEventType& InEventID,
                    const UObject* InTarget,
                    const FName InFunctionName);
    bool UnregisterBP(const FGESEventType& InEventID,
                           const UObject* InTarget,
                           const FName InFunctionName);

    bool Dispatch(const FGESEventDataArray& EventData);

    void Clear();

private:
    TMap<FGESEventType, FGESListeners> EventMap;
};
