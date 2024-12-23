// Copyright Threepeat LLC, 2024. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ThreepeatAnimTools : ModuleRules
{
	public ThreepeatAnimTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(GetModuleDirectory("LevelSequenceEditor"), "Private")
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject", "Engine", "InputCore", "MovieScene", "MovieSceneTracks", "LevelSequence", "Sequencer", "ApplicationCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);

        PrivateIncludePathModuleNames.AddRange(new string[] { "Sequencer" });

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "AssetDefinition",
                "CoreUObject",
				"Engine",
                "EditorFramework",
                "CurveEditor",
                "UnrealEd",
                "Slate",
				"SlateCore",
				"MovieScene",
                "MovieSceneTracks",
                "MovieSceneTools",
                "SequencerCore",
                "Sequencer",
                "LevelSequenceEditor",
				"ClassViewer",
                "AssetTools",
				"LevelSequence",
                "SequencerScripting",
				"ApplicationCore"

				// ... add private dependencies that you statically link with here ...	
			}
            );

	        /*if (Target.bBuildEditor)
		    {
			    PrivateDependencyModuleNames.Add("UnrealEd");
			}*/	


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
