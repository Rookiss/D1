using UnrealBuildTool;

public class D1Target : TargetRules
{
	public D1Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange(new string[] { "D1" });

		ApplySharedD1TargetSettings(this);
	}

	internal static void ApplySharedD1TargetSettings(TargetRules Target)
	{
		Target.DefaultBuildSettings = BuildSettingsVersion.V4;
		Target.IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
	}
}
