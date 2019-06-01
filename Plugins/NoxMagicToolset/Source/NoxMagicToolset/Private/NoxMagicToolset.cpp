// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "NoxMagicToolset.h"

#include "IPluginManager.h"
#include "SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FNoxMagicToolsetModule"

void FNoxMagicToolsetModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	StyleSet = MakeShareable(new FSlateStyleSet("NoxMagicStyle"));

	//Content path of this plugin
	FString ContentDir = IPluginManager::Get().FindPlugin("NoxMagicToolset")->GetBaseDir();

	//The image we wish to load is located inside the Resources folder inside the Base Directory
	//so let's set the content dir to the base dir and manually switch to the Resources folder:
	StyleSet->SetContentRoot(ContentDir);

	//Create a brush from the icon
	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/Tree128"), TEXT(".png")), FVector2D(128.f, 128.f));

	if (ThumbnailBrush)
	{
		//In order to bind the thumbnail to our class we need to type ClassThumbnail.X where X is the name of the C++ class of the asset
		StyleSet->Set("ClassThumbnail.TreeAsset", ThumbnailBrush);

		//Reguster the created style
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}
}

void FNoxMagicToolsetModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//Unregister the style
	FSlateStyleRegistry::UnRegisterSlateStyle(StyleSet->GetStyleSetName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNoxMagicToolsetModule, NoxMagicToolset)