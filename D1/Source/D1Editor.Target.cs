using UnrealBuildTool;

public class D1EditorTarget : TargetRules
{
	public D1EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange(new string[] { "D1" });

		D1Target.ApplySharedD1TargetSettings(this);
	}
}
