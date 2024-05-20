// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class D1ClientTarget : TargetRules
{
	public D1ClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		ExtraModuleNames.AddRange(new string[] { "D1Game" });

		D1GameTarget.ApplySharedD1TargetSettings(this);
	}
}
