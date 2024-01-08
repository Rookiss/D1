using UnrealBuildTool;

public class D1ServerTarget : TargetRules
{
	public D1ServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		ExtraModuleNames.AddRange(new string[] { "D1" });

		D1Target.ApplySharedD1TargetSettings(this);

		bUseChecksInShipping = true;
	}
}
