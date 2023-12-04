#pragma once

#include "GlobalEventHandler.h"
#include "UnLua.h"

/**
 * FGlobalEventSystem
 * 全局事件系统
 */
class GLOBALEVENTSYSTEM_API FGlobalEventSystem final
{
public:
	static void Clear()
	{
		EventIDs.Empty();
		GetHandler().Clear();
	}

	static void Init()
	{
		// Register function 'DispatchEventToCpp'
		lua_State* L = UnLua::GetState();
		if (L)
		{
			lua_register(L, "DispatchEventToCpp", FGlobalEventSystem::ReceiveEventFromLua);
			UE_LOG(LogGlobalEventSystem, Log, TEXT("DispatchEventToCpp is registered ok!"));
		}

		// Get event ID from lua
		const auto LuaRetResult = UnLua::Call(UnLua::GetState(), "Global_GetEventNames");
		if (LuaRetResult.Num() > 0)
		{
			EventIDs = LuaRetResult[0].Value<TArray<FGlobalEventType>>();
			UE_LOG(LogGlobalEventSystem, Log, TEXT("Get EventIDs:%d from lua!"), EventIDs.Num());
		}
	}

	/**
	 * 注册事件
	 * @param InEventID FGameEventType 事件ID
	 * @param InCallback FGameEventStaticDelegate 事件回调
	 */
	static void Register(const FGlobalEventType& InEventID,
	                     const FGlobalEventStaticDelegate InCallback)
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().Register(InEventID, InCallback);
	}

	/**
	 * 反注册事件
	 * - 需要在特定的时期进行反注册，避免无效的事件回调
	 *
	 * @param InEventID FGameEventType 事件ID
	 * @param InCallback FGameEventStaticDelegate 事件回调
	 */
	static void Unregister(const FGlobalEventType& InEventID,
	                       const FGlobalEventStaticDelegate InCallback)
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().Unregister(InEventID, InCallback);
	}

	/**
	 * 注册事件(UObject)
	 * @param InEventID FGameEventType 事件ID
	 * @param InTarget ClassType* 目标对象(必须是UObject)
	 * @param InCallback ClassType::*InCallback 目标成员函数回调
	 */
	template <typename ClassType>
	static void Register(const FGlobalEventType& InEventID,
	                     std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	                     void (ClassType::*InCallback)(const FGlobalEventData&))
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().Register(InEventID, InTarget, InCallback);
	}

	/**
	 * 反注册事件
	 * - 需要在特定的时期进行反注册，避免无效的事件回调
	 *
	 * @param InEventID FGameEventType 事件ID
	 * @param InTarget ClassType 目标对象
	 * @param InCallback ClassType::*InCallback 目标成员函数回调
	 */
	template <typename ClassType>
	static void Unregister(const FGlobalEventType& InEventID,
	                       std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	                       void (ClassType::*InCallback)(const FGlobalEventData&))
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().Unregister(InEventID, InTarget, InCallback);
	}

	/**
	 * 注册事件(非UObject使用, 可能存在不安全性)
	 * @param InEventID FGameEventType 事件ID
	 * @param InTarget ClassType* 目标对象(非UObject)
	 * @param InCallback ClassType::*InCallback 目标成员函数回调
	 */
	template <typename ClassType>
	static void RegisterRaw(const FGlobalEventType& InEventID,
	                        ClassType* InTarget,
	                        void (ClassType::*InCallback)(const FGlobalEventData&))
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().RegisterRaw(InEventID, InTarget, InCallback);
	}

	/**
	 * 反注册事件(非UObject使用, 可能存在不安全性)
	 * - 需要在特定的时期进行反注册，避免无效的事件回调
	 *
	 * @param InEventID FGameEventType 事件ID
	 * @param InTarget ClassType 目标对象
	 * @param InCallback ClassType::*InCallback 目标成员函数回调
	 */
	template <typename ClassType>
	static void UnregisterRaw(const FGlobalEventType& InEventID,
	                          ClassType* InTarget,
	                          void (ClassType::*InCallback)(const FGlobalEventData&))
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().UnregisterRaw(InEventID, InTarget, InCallback);
	}

	/**
	 * 触发事件到所有层(C++/Lua)
	 * @param EventID FGameEventType 事件ID
	 * @param Args T&&... 事件数据, 变参
	 */
	template <typename... T>
	static void DispatchToAll(const FGlobalEventType& EventID, T&&... Args)
	{
		DispatchToCpp(EventID, Forward<T>(Args)...);
		DispatchToLua(EventID, Forward<T>(Args)...);
	}

	/**
	 * 触发事件(仅C++)
	 * @param EventID FGameEventType 事件ID
	 * @param Args T&&... 事件数据, 变参
	 */
	template <typename... T>
	static void DispatchToCpp(const FGlobalEventType& EventID, T&&... Args)
	{
		FGlobalEventData EventData;
		EventData.EventID = EventID;

		PushParams(EventData, Forward<T>(Args)...);

		DispatchToCpp(EventData);
	}

	/**
	 * 触发事件(仅C++)
	 * @param EventData FGameEventData 事件数据对象
	 */
	static void DispatchToCpp(const FGlobalEventData& EventData)
	{
		GetHandler().Dispatch(EventData);
	}

	/**
	 * 触发事件(仅Lua)
	 * @param EventID FGameEventType 事件ID
	 * @param Args T 事件数据, 变参
	 */
	template <typename... T>
	static void DispatchToLua(const FGlobalEventType& EventID, T&&... Args)
	{
		UnLua::CallTableFunc(UnLua::GetState(), "EventSystem", "Trigger", EventID, Forward<T>(Args)...);
	}

private:
	static void PushParams(FGlobalEventData& EventData)
	{
	}

	template <typename T1, typename... T2>
	static void PushParams(FGlobalEventData& EventData, T1&& V, T2&&... Args)
	{
		EventData.PushParam(Forward<T1>(V));
		PushParams(EventData, Forward<T2>(Args)...);
	}

private:
	FGlobalEventSystem()
	{
	}

private:
	static FGlobalEventHandler& GetHandler()
	{
		static FGlobalEventHandler Handler;
		return Handler;
	}

	static bool CheckLuaParams(lua_State* L);

	static bool CheckEventID(const FGlobalEventType& EventID)
	{
		if (IsRunningCommandlet())
		{
			return false;
		}

		if (!EventIDs.Contains(EventID))
		{
			UE_LOG(LogGlobalEventSystem, Error, TEXT("CheckEventID: EventID:%s isn't define in 'Enums.lua'!"), *EventID.ToString());
			return false;
		}

		return true;
	}

	static bool ParseEventData(lua_State* L, FGlobalEventData& EventData);

	static int ReceiveEventFromLua(lua_State* L);

	static TArray<FGlobalEventType> EventIDs;
};