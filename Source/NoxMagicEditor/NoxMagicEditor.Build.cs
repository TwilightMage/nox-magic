using UnrealBuildTool;

public class NoxMagicEditor : ModuleRules
{
	public NoxMagicEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                "NoxMagicEditor/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[]
            {
                "NoxMagicEditor/Private"
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "NoxMagic",
                "AssetTools"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            });

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            });

        bEnableExceptions = true;

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
