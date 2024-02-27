#pragma once

#include "GESHandler.h"
#include "UnLua.h"

/**
 * FGES
 * Global Event System
 */
class GES_API FGES final
{
public:
	/**
	 * @brief Init GES
	 */
	static void Init()
	{
		if (IsRunningCommandlet())
		{
			return;
		}
		
		if (bInit) return;
		
		if (!UnLua::Startup())
		{
			UE_LOG(LogGES, Error, TEXT("Failed to call 'UnLua::Startup'"));
			return;
		}

		lua_State* L = UnLua::GetState();
		if (!UnLua::RunFile(L, TEXT("Preload.lua")))
		{
			UE_LOG(LogGES, Error, TEXT("Failed to call 'Preload.lua'"));
			return;
		}
		
		if (!FGESEventConfigHelper::Init())
		{
			UE_LOG(LogGES, Error, TEXT("Failed to init 'EventConfig'"));
			return;			
		}

		UE_LOG(LogGES, Verbose, TEXT("FGES::Init OK!"));

		bInit = true;
	}

	/**
	 * @brief Shutdown GES
	 */
	static void Shutdown(const UObject* WorldContext = nullptr)
	{
		if (IsRunningCommandlet())
		{
			return;
		}
		
		if (!bInit) return;
		
		UnLua::Shutdown();
		
		GetHandler(WorldContext).Clear();

#if !WITH_EDITOR
		FGESEventConfigHelper::Clear();
#endif

		UE_LOG(LogGES, Verbose, TEXT("FGES::Shutdown OK!"));
		bInit = false;
	}

	/**
	 * @brief Register event for static function
	 * @param InEventID Event ID
	 * @param InCallback Static event callback
	 */
	static void RegisterStatic(const FGESEventType& InEventID,
	                     const FGESStaticDelegate InCallback)
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().RegisterStatic(InEventID, InCallback);
	}

	/**
	 * @brief Unregister event for static function
	 * @param InEventID Event ID
	 * @param InCallback Static event callback
	 */
	static void UnregisterStatic(const FGESEventType& InEventID,
	                       const FGESStaticDelegate InCallback)
	{
		if (!CheckEventID(InEventID))
		{
			return;
		}

		GetHandler().UnregisterStatic(InEventID, InCallback);
	}

	/**
	 * @brief Register event for UObject
	 * @param InEventID Event ID
	 * @param InTarget Target object(Must be UObject)
	 * @param InCallback ClassType::*InCallback Event callback
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

		GetHandler(InTarget).Register(InEventID, InTarget, InCallback);
	}

	/**
	 * @brief Unregister event for UObject
	 * @param InEventID Event ID
	 * @param InTarget Target object(Must be UObject)
	 * @param InCallback ClassType::*InCallback Event callback
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

		GetHandler(InTarget).Unregister(InEventID, InTarget, InCallback);
	}

	/**
	 * @brief Register event for None-UObject
	 * @param InEventID Event ID
	 * @param InTarget Target object(None-UObject)
	 * @param InCallback ClassType::*InCallback Event callback
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
	 * @brief Unregister event for None-UObject
	 * @param InEventID Event ID
	 * @param InTarget Target object(None-UObject)
	 * @param InCallback ClassType::*InCallback Event callback
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
	 * @brief Register event for blueprint
	 * @param InEventID Event ID
	 * @param InTarget Target Object
	 * @param InFunctionName Event callback name in 'InTarget', must be UFunction
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

		GetHandler(InTarget).RegisterBP(InEventID, InTarget, InFunctionName);
	}

	/**
	 * @brief Unregister event for blueprint
	 * @param InEventID Event ID
	 * @param InTarget Target Object
	 * @param InFunctionName Event callback name in 'InTarget', must be UFunction
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

		GetHandler(InTarget).UnregisterBP(InEventID, InTarget, InFunctionName);
	}

	/**
	 * @brief Dispatch event from blueprint
	 * @param EventData Event data array
	 */
	static void DispatchBP(const FGESEventDataArray& EventData)
	{
		Dispatch(EventData);
	}

	/**
	 * @brief Dispatch event from C++
	 * @param EventID Event ID
	 * @param Args T&&... Variadic params
	 */
	template <typename... T>
	static void Dispatch(const FGESEventType& EventID, T&&... Args)
	{
		FGESEventDataArray EventData;
		EventData.SetEventID(EventID);

		PushParams(EventData, Forward<T>(Args)...);

		Dispatch(EventData);
	}

	/**
	 * @brief Dispatch event from C++
	 * @param EventData Event data array
	 */
	static void Dispatch(const FGESEventDataArray& EventData)
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

	static FGESHandler& GetHandler(const UObject* WorldContext = nullptr)
	{
#if WITH_EDITOR
		// Separate 'GESHandler' between server and client, because there may exist server and client at the same time 
		static FGESHandler ServerHandler, ClientHandler;

		const UWorld* CurrentWorld = WorldContext ? WorldContext->GetWorld() : GEngine->GetCurrentPlayWorld();
		if (!CurrentWorld)
		{
			UE_LOG(LogGES, Warning, TEXT("FGES::GetHandler GEngine->GetCurrentPlayWorld Null, this may triggers wrong logic!"));
			const bool bIsServer = IsRunningDedicatedServer();
			return bIsServer ? ServerHandler : ClientHandler;
		}

		const ENetMode NetMode = CurrentWorld->GetNetMode();
		UE_CLOG(NetMode == NM_MAX, LogGES, Warning, TEXT("FGES::GetHandler NetMode:Invalid value!"));

		// Standalone/DS/LS return ServerHandler
		if (NetMode == NM_Standalone || NetMode == NM_DedicatedServer || NetMode == NM_ListenServer)
		{
			return ServerHandler;
		}

		// Client return 'ClientHandler'
		return ClientHandler;
#else
		static FGESHandler Handler;
		return Handler;
#endif
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