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

    bool Register(const FGESStaticDelegate& InCallback);
    bool Unregister(const FGESStaticDelegate& InCallback);

    template<typename ClassType>
    bool Register(
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&));
    template<typename ClassType>
    bool Unregister(
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&));

    template<typename ClassType>
    bool RegisterRaw(ClassType* InTarget, void (ClassType::* InCallback) (const FGESEventDataArray&));
    template<typename ClassType>
    bool UnregisterRaw(ClassType* InTarget, void (ClassType::* InCallback) (const FGESEventDataArray&));
    
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
    bool Register(
        const FGESEventType& InEventID,
        const FGESStaticDelegate InStaticCallback);
    bool Unregister(
        const FGESEventType& InEventID,
        const FGESStaticDelegate InStaticCallback);

    template<typename ClassType>
    bool Register(
        const FGESEventType& InEventID,
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&));
    template<typename ClassType>
    bool Unregister(
        const FGESEventType& InEventID,
        std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&));

    template<typename ClassType>
    bool RegisterRaw(
        const FGESEventType& InEventID, ClassType* InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&));
    template<typename ClassType>
    bool UnregisterRaw(
        const FGESEventType& InEventID, ClassType* InTarget,
        void (ClassType::* InCallback) (const FGESEventDataArray&));

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
