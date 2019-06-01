

using UnrealBuildTool;
using System.Collections.Generic;

public class NoxMagicTarget : TargetRules
{
	public NoxMagicTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "NoxMagic" } );
	}
}
