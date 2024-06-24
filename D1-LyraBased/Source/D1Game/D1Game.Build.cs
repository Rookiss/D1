using UnrealBuildTool;

public class D1Game : ModuleRules
{
	public D1Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] 
		{
			"D1Game"
		});

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core",
			"CoreOnline",
			"CoreUObject",
			"ApplicationCore",
			"Engine",
			"PhysicsCore",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			"AIModule",
			"ModularGameplay",
			"ModularGameplayActors",
			"DataRegistry",
			"ReplicationGraph",
			"GameFeatures",
			"SignificanceManager",
			"Hotfix",
			"CommonLoadingScreen",
			"Niagara",
			"AsyncMixin",
			"ControlFlows",
			"PropertyPath",
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"OnlineSubsystemUtils",
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"InputCore",
			"Slate",
			"SlateCore",
			"RenderCore",
			"DeveloperSettings",
			"EnhancedInput",
			"NetCore",
			"RHI",
			"Projects",
			"Gauntlet",
			"UMG",
			"CommonUI",
			"CommonInput",
			"GameSettings",
			"CommonGame",
			"CommonUser",
			"GameSubtitles",
			"GameplayMessageRuntime",
			"AudioMixer",
			"NetworkReplayStreaming",
			"UIExtension",
			"ClientPilot",
			"AudioModulation",
			"EngineSettings",
			"DTLSHandlerComponent", 
			"PocketWorlds",
			"NavigationSystem",
		});
		
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
		
		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
