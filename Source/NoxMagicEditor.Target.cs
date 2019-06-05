

using UnrealBuildTool;
using System.Collections.Generic;

public class NoxMagicEditorTarget : TargetRules
{
	public NoxMagicEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "NoxMagicEditor" } );
	}
}
