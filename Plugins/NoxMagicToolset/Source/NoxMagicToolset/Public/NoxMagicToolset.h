// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "SlateStyle.h"

class FNoxMagicToolsetModule : public IModuleInterface
{
public:
	TSharedPtr<FSlateStyleSet> StyleSet;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
