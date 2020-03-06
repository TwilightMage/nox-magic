//  Copyright 2018 (C) Bruno Xavier B. Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {
//
public class USQLite : ModuleRules {
    public USQLite(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Public/USQLite.h";
		bEnforceIWYU = true;
		//
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"Json",
			"Slate",
			"Engine",
			"SlateCore",
			"CoreUObject",
			"MoviePlayer",
			"GameplayTags",
			"JsonUtilities"
		});
		//
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"Private"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"ThirdParty/SQLite/Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"ThirdParty/SQLite/Private"));
		//
		if (Target.Platform == UnrealTargetPlatform.Android) {
			PrivateDefinitions.Add("_FILE_OFFSET_BITS=32");
		}///
		//
		PrivateDefinitions.Add("SQLITE_HAVE_ISNAN=1");
		PrivateDefinitions.Add("SQLITE_ENABLE_FTS4");
		PrivateDefinitions.Add("SQLITE_ENABLE_FTS5");
		//
		if (Target.bCompileICU) {
			PrivateDefinitions.Add("SQLITE_ENABLE_ICU");
			AddEngineThirdPartyPrivateStaticDependencies(Target,"ICU");
		}///
	}///
}///
//
}