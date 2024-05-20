// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class D1ServerTarget : TargetRules
{
	public D1ServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		ExtraModuleNames.AddRange(new string[] { "D1Game" });

		D1GameTarget.ApplySharedD1TargetSettings(this);

		bUseChecksInShipping = true;
	}
}
