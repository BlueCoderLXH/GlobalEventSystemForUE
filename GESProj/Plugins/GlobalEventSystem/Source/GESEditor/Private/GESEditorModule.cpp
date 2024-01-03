#include "GESEditorModule.h"

DEFINE_LOG_CATEGORY(LogGESEditor)

#define LOCTEXT_NAMESPACE "GlobalEventSystemEditor"

IMPLEMENT_GAME_MODULE(FGESEditorModule, GESEditor);

void FGESEditorModule::StartupModule()
{
	
	UE_LOG(LogGESEditor, Warning, TEXT("StartupModule"));
}

void FGESEditorModule::ShutdownModule()
{
	UE_LOG(LogGESEditor, Warning, TEXT("ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE