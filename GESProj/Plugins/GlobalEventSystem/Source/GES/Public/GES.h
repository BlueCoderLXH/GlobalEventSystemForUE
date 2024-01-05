#pragma once

#include "GESHandler.h"
#include "UnLua.h"

/**
 * FGES
 * Global event system
 */
class GES_API FGES final
{
public:
	static void Init()
	{
		if (bInit) return;
		
		UnLua::Startup();

		lua_State* L = UnLua::GetState();
		UnLua::RunFile(L, TEXT("Preload.lua"));
		
		FGESEventConfigHelper::Init();

		bInit = true;
	}

	static void Clear()
	{
		if (!bInit) return;
		
		UnLua::Shutdown();
		
		GetHandler().Clear();

#if !WITH_EDITOR
		FGESEventConfigHelper::Clear();
#endif

		bInit = false;
	}

	/**
	 * 注册事件
	 * @param InEventID FGameEventType 事件ID
	 * @param InCallback FGameEventStaticDelegate 事件回调
	 */
	static void Register(const FGESEventType& InEventID,
	                     const FGESStaticDelegate InCallback)
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
	static void Unregister(const FGESEventType& InEventID,
	                       const FGESStaticDelegate InCallback)
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
	static void Register(const FGESEventType& InEventID,
	                     std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	                     void (ClassType::*InCallback)(const FGESEventDataArray&))
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
	static void Unregister(const FGESEventType& InEventID,
	                       std::enable_if_t<std::is_base_of<UObject, ClassType>::value, ClassType*> InTarget,
	                       void (ClassType::*InCallback)(const FGESEventDataArray&))
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
	static void RegisterRaw(const FGESEventType& InEventID,
	                        ClassType* InTarget,
	                        void (ClassType::*InCallback)(const FGESEventDataArray&))
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
	static void UnregisterRaw(const FGESEventType& InEventID,
	                          ClassType* InTarget,
	                          void (ClassType::*InCallback)(const FGESEventDataArray&))
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().UnregisterRaw(InEventID, InTarget, InCallback);
	}

	/**
	 * @brief 注册蓝图函数
	 * @param InEventID FGameEventType 事件ID
	 * @param InTarget 目标UObject对象
	 * @param InFunctionName UObject绑定的UFunction名
	 */
	static void RegisterBP(const FGESEventType& InEventID,
	                       const UObject* InTarget,
	                       const FName InFunctionName)
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		check(IsValid(InTarget));

		GetHandler().RegisterBP(InEventID, InTarget, InFunctionName);
	}

	/**
	 * @brief 反注册蓝图函数
	 * @param InEventID FGameEventType 事件ID
	 * @param InTarget 目标UObject对象
	 * @param InFunctionName UObject绑定的UFunction名
	 */
	static void UnregisterBP(const FGESEventType& InEventID,
	                         const UObject* InTarget,
	                         const FName InFunctionName)
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		check(IsValid(InTarget));

		GetHandler().UnregisterBP(InEventID, InTarget, InFunctionName);
	}

	/**
	 * 触发事件(蓝图层)
	 * @param EventData FGameEventData 事件数据对象
	 */
	static void DispatchFromBP(const FGESEventDataArray& EventData)
	{
		DispatchToCpp(EventData);
	}	

	/**
	 * 触发事件到所有层(C++/Lua/BP)
	 * @param EventID FGameEventType 事件ID
	 * @param Args T&&... 事件数据, 变参
	 */
	template <typename... T>
	static void DispatchToAll(const FGESEventType& EventID, T&&... Args)
	{
		DispatchToCpp(EventID, Forward<T>(Args)...);
	}

	/**
	 * 触发事件(仅C++)
	 * @param EventID FGameEventType 事件ID
	 * @param Args T&&... 事件数据, 变参
	 */
	template <typename... T>
	static void DispatchToCpp(const FGESEventType& EventID, T&&... Args)
	{
		FGESEventDataArray EventData;
		EventData.SetEventID(EventID);

		PushParams(EventData, Forward<T>(Args)...);

		DispatchToCpp(EventData);
	}

	/**
	 * 触发事件(仅C++)
	 * @param EventData FGameEventData 事件数据对象
	 */
	static void DispatchToCpp(const FGESEventDataArray& EventData)
	{
		GetHandler().Dispatch(EventData);
	}

private:
	static void PushParams(FGESEventDataArray& EventData)
	{
	}

	template <typename T1, typename... T2>
	static void PushParams(FGESEventDataArray& EventData, T1&& V, T2&&... Args)
	{
		EventData.PushParam(Forward<T1>(V));
		PushParams(EventData, Forward<T2>(Args)...);
	}

private:
	FGES()
	{
	}

	static FGESHandler& GetHandler()
	{
		static FGESHandler Handler;
		return Handler;
	}

	static bool CheckLuaParams(lua_State* L);

	static bool CheckEventID(const FGESEventType& EventID)
	{
		if (IsRunningCommandlet())
		{
			return false;
		}

		if (!FGESEventConfigHelper::FindEvent(EventID))
		{
			UE_LOG(LogGES, Error, TEXT("CheckEventID: EventID:%s isn't define in 'Enums.lua'!"), *EventID.ToString());
			return false;
		}

		return true;
	}

	static bool bInit;
};