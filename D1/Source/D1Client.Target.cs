using UnrealBuildTool;

public class D1ClientTarget : TargetRules
{
	public D1ClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		ExtraModuleNames.AddRange(new string[] { "D1" });

		D1Target.ApplySharedD1TargetSettings(this);
	}
}
