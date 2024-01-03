#include "GESEventData.h"

#include "LuaCore.h"
#include "UnLuaEx.h"

int GES_luaL_error (lua_State *L, const char *fmt, ...)
{
	va_list Args;
	va_start(Args, fmt);
	const int32 RetValue = luaL_error(L, "%s %s", "[GES for lua]", fmt, Args);
	va_end(Args);
	return RetValue;
}

FGESEventDataArray* FGESEventDataArray_GetSelf(lua_State* L, const int32 InNumParams)
{
	const int32 NumParams = lua_gettop(L);
	if (NumParams != InNumParams)
	{
		GES_luaL_error(L, "Invalid parameters");
		return nullptr;
	}

	FGESEventDataArray* EventDataArray = static_cast<FGESEventDataArray*>(GetCppInstanceFast(L, 1));
	if (!EventDataArray)
	{
		GES_luaL_error(L, "First param isn't a valid FGESEventDataArray");
		return nullptr;
	}

	return EventDataArray;
}

static int32 FGESEventDataArray_SetEventID(lua_State* L)
{
	// [1] = Self, [2] = EventID
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const FName EventID = UTF8_TO_TCHAR(lua_tostring(L, 2));
	EventDataArray->SetEventID(EventID);

	return 1;
}

static int32 FGESEventDataArray_PushBool(lua_State* L)
{
	static bool InternalBool; // avoid destruction
	
	// [1] = Self, [2] = bool
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalBool = lua_toboolean(L, 2) ? true : false;
	EventDataArray->PushParam(InternalBool);
	return 1;
}

static int32 FGESEventDataArray_PushInteger(lua_State* L)
{
	static int32 InternalInt; // avoid destruction
	
	// [1] = Self, [2] = int
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalInt = lua_tointeger(L, 2);
	EventDataArray->PushParam(InternalInt);
	return 1;
}

static int32 FGESEventDataArray_PushFloat(lua_State* L)
{
	static double InternalFloat; // avoid destruction
	
	// [1] = Self, [2] = int
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalFloat = lua_tonumber(L, 2);
	EventDataArray->PushParam(InternalFloat);
	return 1;
}

static int32 FGESEventDataArray_PushString(lua_State* L)
{
	static FString InternalStr; // avoid destruction
	
	// [1] = Self, [2] = String
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalStr = UTF8_TO_TCHAR(lua_tostring(L, 2));
	EventDataArray->PushParam(InternalStr);
	return 1;
}

static int32 FGESEventDataArray_PushName(lua_State* L)
{
	static FName InternalName; // avoid destruction
	
	// [1] = Self, [2] = String
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalName = UTF8_TO_TCHAR(lua_tostring(L, 2));
	EventDataArray->PushParam(InternalName);
	return 1;
}

static int32 FGESEventDataArray_PushText(lua_State* L)
{
	static FText InternalText; // avoid destruction
	
	// [1] = Self, [2] = String
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalText = FText::FromString(UTF8_TO_TCHAR(lua_tostring(L, 2)));
	EventDataArray->PushParam(InternalText);
	return 1;
}

static int32 FGESEventDataArray_PushEnum(lua_State* L)
{
	static int32 InternalInt; // avoid destruction
	
	// [1] = Self, [2] = UEnum(int)
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	InternalInt = lua_tointeger(L, 2);
	EventDataArray->PushParamWithType({EGESCppType::UEnum}, &InternalInt);
	return 1;
}

static int32 FGESEventDataArray_PushStruct(lua_State* L)
{
	// [1] = Self, [2] = UStruct
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	const void* StructRawPtr = UnLua::GetPointer(L, 2);
	EventDataArray->PushParamWithType({EGESCppType::UStruct}, StructRawPtr);
	return 1;
}

static int32 FGESEventDataArray_PushObject(lua_State* L)
{
	// [1] = Self, [2] = UObject
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	UObject* ObjPtr = UnLua::GetUObject(L, 2);
	EventDataArray->PushParam(ObjPtr);
	return 1;
}

static int32 FGESEventDataArray_PushArray(lua_State* L)
{
	// [1] = Self, [2] = TArray
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	const void* ArrayRawPtr = UnLua::GetScriptContainerPointer(L, 2);
	EventDataArray->PushParamWithType({EGESCppType::None, EGESContainerType::Array}, ArrayRawPtr);
	return 1;
}

static int32 FGESEventDataArray_PushMap(lua_State* L)
{
	// [1] = Self, [2] = TMap
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	const void* MapRawPtr = UnLua::GetScriptContainerPointer(L, 2);
	EventDataArray->PushParamWithType({EGESCppType::None, EGESContainerType::Map}, MapRawPtr);
	return 1;
}

static int32 FGESEventDataArray_PushSet(lua_State* L)
{
	// [1] = Self, [2] = TSet
	FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);
	const void* SetRawPtr = UnLua::GetScriptContainerPointer(L, 2);
	EventDataArray->PushParamWithType({EGESCppType::None, EGESContainerType::Map}, SetRawPtr);
	return 1;
}

static int32 FGESEventDataArray_GetBool(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const bool BoolValue = EventDataArray->GetBool(EventIndex);

	lua_pushboolean(L, BoolValue);

	return 1;
}

static int32 FGESEventDataArray_GetInteger(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const int32 IntValue = EventDataArray->GetInt(EventIndex);

	lua_pushinteger(L, IntValue);

	return 1;
}

static int32 FGESEventDataArray_GetFloat(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const float FloatValue = EventDataArray->GetFloat(EventIndex);

	lua_pushnumber(L, FloatValue);

	return 1;
}

static int32 FGESEventDataArray_GetString(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const FString& StrValue = EventDataArray->GetString(EventIndex);

	lua_pushstring(L, TCHAR_TO_UTF8(*StrValue));

	return 1;
}

static int32 FGESEventDataArray_GetName(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const FName& NameValue = EventDataArray->GetName(EventIndex);

	lua_pushstring(L, TCHAR_TO_UTF8(*NameValue.ToString()));

	return 1;
}

static int32 FGESEventDataArray_GetText(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const FText& TextValue = EventDataArray->GetText(EventIndex);

	lua_pushstring(L, TCHAR_TO_UTF8(*TextValue.ToString()));

	return 1;
}

static int32 FGESEventDataArray_GetEnum(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	const int32 EnumIntValue = EventDataArray->GetEnum<int32>(EventIndex);

	lua_pushinteger(L, EnumIntValue);

	return 1;
}

static int32 FGESEventDataArray_GetStruct(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	void* StructRawPtr = const_cast<void*>(EventDataArray->GetStructRawPtr(EventIndex));
	const FString StructTypeName = EventDataArray->GetParamSubType(EventIndex).ToString();
	const FTCHARToUTF8 StructCharName(*FString::Printf(TEXT("F%s"), *StructTypeName));

	UnLua::PushPointer(L, StructRawPtr, StructCharName.Get(), false);
	return 1;
}

static int32 FGESEventDataArray_GetObject(lua_State* L)
{
	// [1] = Self, [2] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 2);

	const int32 EventIndex = lua_tointeger(L, 2);
	UObject* ObjPtr = EventDataArray->GetUObject<UObject>(EventIndex);

	return UnLua::PushUObject(L, ObjPtr);
}

static int32 FGESEventDataArray_GetArray(lua_State* L)
{
	// [1] = Self, [2] = ElementType, [3] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 3);

	const int32 EventIndex = lua_tointeger(L, 3);
	const void* SourceArrayPtr = EventDataArray->GetContainerRawPtr(EventIndex);
	if (!SourceArrayPtr)
	{
		return GES_luaL_error(L, "First param isn't a valid TArray");
	}

	const auto& Env = UnLua::FLuaEnv::FindEnvChecked(L);
	const auto ElementType = Env.GetPropertyRegistry()->CreateTypeInterface(L, 2);
	if (!ElementType)
	{
		return GES_luaL_error(L, "Failed to parse element type for TArray");
	}

	if (!PushArray(L, static_cast<const FScriptArray*>(SourceArrayPtr), ElementType, false))
	{
		return GES_luaL_error(L, "Failed to push array for TArray");
	}
	
	return 1;
}

static int32 FGESEventDataArray_GetMap(lua_State* L)
{
	// [1] = Self, [2] = KeyType, [3] = ValueType, [4] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 4);

	const int32 EventIndex = lua_tointeger(L, 4);
	const void* SourceMapPtr = EventDataArray->GetContainerRawPtr(EventIndex);
	if (!SourceMapPtr)
	{
		return GES_luaL_error(L, "First param isn't a valid TMap");
	}

	const auto& Env = UnLua::FLuaEnv::FindEnvChecked(L);
	const auto KeyType = Env.GetPropertyRegistry()->CreateTypeInterface(L, 2);
	if (!KeyType)
	{
		return GES_luaL_error(L, "Failed to parse key type for TMap");
	}
	
	const auto ValueType = Env.GetPropertyRegistry()->CreateTypeInterface(L, 3);
	if (!ValueType)
	{
		return GES_luaL_error(L, "Failed to parse value type for TMap");
	}

	if (!PushMap(L, static_cast<const FScriptMap*>(SourceMapPtr), KeyType, ValueType, false))
	{
		return GES_luaL_error(L, "Failed to push array for TMap");
	}
	
	return 1;
}

static int32 FGESEventDataArray_GetSet(lua_State* L)
{
	// [1] = Self, [2] = ElementType, [3] = Index
	const FGESEventDataArray* EventDataArray = FGESEventDataArray_GetSelf(L, 3);

	const int32 EventIndex = lua_tointeger(L, 3);
	const void* SourceSetPtr = EventDataArray->GetContainerRawPtr(EventIndex);
	if (!SourceSetPtr)
	{
		return GES_luaL_error(L, "First param isn't a valid TSet");
	}

	const auto& Env = UnLua::FLuaEnv::FindEnvChecked(L);
	const auto ElementType = Env.GetPropertyRegistry()->CreateTypeInterface(L, 2);
	if (!ElementType)
	{
		return GES_luaL_error(L, "Failed to parse element type for TSet");
	}

	if (!PushSet(L, static_cast<const FScriptSet*>(SourceSetPtr), ElementType, false))
	{
		return GES_luaL_error(L, "Failed to push array for TSet");
	}

	return 1;
}

static const luaL_Reg FGESEventDataArrayLib[] =
{
	{"SetEventID",	FGESEventDataArray_SetEventID	},

	{"PushBool",		FGESEventDataArray_PushBool	},
	{"PushInteger",	FGESEventDataArray_PushInteger	},
	{"PushFloat",		FGESEventDataArray_PushFloat	},
	{"PushString",	FGESEventDataArray_PushString	},
	{"PushFName",		FGESEventDataArray_PushName	},
	{"PushText",		FGESEventDataArray_PushText	},
	{"PushEnum",		FGESEventDataArray_PushEnum	},
	{"PushStruct",	FGESEventDataArray_PushStruct	},
	{"PushObject",	FGESEventDataArray_PushObject	},
	{"PushArray",		FGESEventDataArray_PushArray	},
	{"PushMap",		FGESEventDataArray_PushMap		},
	{"PushSet",		FGESEventDataArray_PushSet		},
	
	{"GetBool",		FGESEventDataArray_GetBool		},
	{"GetInteger",	FGESEventDataArray_GetInteger	},
	{"GetFloat",		FGESEventDataArray_GetFloat	},
	{"GetString",		FGESEventDataArray_GetString	},
	{"GetFName",		FGESEventDataArray_GetName		},
	{"GetText",		FGESEventDataArray_GetText		},
	{"GetEnum",		FGESEventDataArray_GetEnum		},
	{"GetStruct",		FGESEventDataArray_GetStruct	},
	{"GetObject",		FGESEventDataArray_GetObject	},
	{"GetArray",		FGESEventDataArray_GetArray	},
	{"GetMap",		FGESEventDataArray_GetMap		},
	{"GetSet",		FGESEventDataArray_GetSet		},
	{nullptr,			nullptr						}// End Identifier
};

EXPORT_UNTYPED_CLASS(FGESEventDataArray, true, FGESEventDataArrayLib)

IMPLEMENT_EXPORTED_CLASS(FGESEventDataArray)