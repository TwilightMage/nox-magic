#include "NoxMagicEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "SlateStyleRegistry.h"
#include "Paths.h"

IMPLEMENT_GAME_MODULE(FNoxMagicEditorModule, NoxMagicEditor);

DEFINE_LOG_CATEGORY(NoxMagicEditor)

#define LOCTEXT_NAMESPACE "NoxMagicEditor"

void FNoxMagicEditorModule::StartupModule()
{
	StyleSet = MakeShareable(new FSlateStyleSet("NoxMagicStyle"));

	StyleSet->SetContentRoot(FPaths::ProjectDir() + "Editor content/");

	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/Tree128"), TEXT(".png")), FVector2D(128.f, 128.f));

	if (ThumbnailBrush)
	{
		StyleSet->Set("ClassThumbnail.TreeAsset", ThumbnailBrush);

		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}
}

void FNoxMagicEditorModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(StyleSet->GetStyleSetName());
}

#undef LOCTEXT_NAMESPACE