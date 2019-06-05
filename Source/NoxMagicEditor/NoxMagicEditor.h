#pragma once

#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "CoreMinimal.h"
#include "SlateStyle.h"

DECLARE_LOG_CATEGORY_EXTERN(NoxMagicEditor, All, All)

class FNoxMagicEditorModule : public IModuleInterface
{
public:
	TSharedPtr<FSlateStyleSet> StyleSet;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};