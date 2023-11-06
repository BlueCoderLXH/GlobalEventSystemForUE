#include "..\Public\GlobalEventSystemEditorModule.h"

DEFINE_LOG_CATEGORY(LogGlobalEventSystemEditor)

#define LOCTEXT_NAMESPACE "GlobalEventSystemEditor"

IMPLEMENT_GAME_MODULE(FGlobalEventSystemEditorModule, GlobalEventSystemEditor);

void FGlobalEventSystemEditorModule::StartupModule()
{
	
	UE_LOG(LogGlobalEventSystemEditor, Warning, TEXT("StartupModule"));
}

void FGlobalEventSystemEditorModule::ShutdownModule()
{
	UE_LOG(LogGlobalEventSystemEditor, Warning, TEXT("ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE