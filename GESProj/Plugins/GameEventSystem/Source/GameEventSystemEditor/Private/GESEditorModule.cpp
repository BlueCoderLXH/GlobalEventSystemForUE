#include "..\Public\GESEditorModule.h"

DEFINE_LOG_CATEGORY(LogGESEditor)

#define LOCTEXT_NAMESPACE "GameEventSystemEditor"

IMPLEMENT_GAME_MODULE(FGESEditorModule, GameEventSystemEditor);

void FGESEditorModule::StartupModule()
{
	
	UE_LOG(LogGESEditor, Warning, TEXT("FGESEditorModule::StartupModule"));
}

void FGESEditorModule::ShutdownModule()
{
	UE_LOG(LogGESEditor, Warning, TEXT("FGESEditorModule::ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE