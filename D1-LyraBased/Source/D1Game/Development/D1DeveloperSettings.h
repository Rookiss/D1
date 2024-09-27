// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UObject/PrimaryAssetId.h"
#include "UObject/SoftObjectPath.h"
#include "D1DeveloperSettings.generated.h"

struct FPropertyChangedEvent;

class ULyraExperienceDefinition;

UENUM()
enum class ECheatExecutionTime
{
	// When the cheat manager is created
	OnCheatManagerCreated,

	// When a pawn is possessed by a player
	OnPlayerPawnPossession
};

USTRUCT()
struct FLyraCheatToRun
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ECheatExecutionTime Phase = ECheatExecutionTime::OnPlayerPawnPossession;

	UPROPERTY(EditAnywhere)
	FString Cheat;
};

/**
 * Developer settings / editor cheats
 */
UCLASS(config=EditorPerProjectUserSettings, MinimalAPI)
class UD1DeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UD1DeveloperSettings();

	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=D1)
	bool bForceDisableDebugTrace = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=D1)
	bool bForceDisableCooldown = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=D1)
	bool bForceDisableCost = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category="D1 Monster")
	bool bAllowSpawnMonsters = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category="D1 Monster")
	bool bAllowMonstersAttackPlayer = true;
	
	// The experience override to use for Play in Editor (if not set, the default for the world settings of the open map will be used)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=Lyra, meta=(AllowedTypes="LyraExperienceDefinition"))
	FPrimaryAssetId ExperienceOverride;
	
	// Do the full game flow when playing in the editor, or skip 'waiting for player' / etc... game phases?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=Lyra)
	bool bTestFullGameFlowInPIE = false;

	/**
	* Should force feedback effects be played, even if the last input device was not a gamepad?
	* The default behavior in Lyra is to only play force feedback if the most recent input device was a gamepad.
	*/
	UPROPERTY(config, EditAnywhere, Category = Lyra, meta = (ConsoleVariable = "LyraPC.ShouldAlwaysPlayForceFeedback"))
	bool bShouldAlwaysPlayForceFeedback = false;

	// Should game logic load cosmetic backgrounds in the editor or skip them for iteration speed?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category=Lyra)
	bool bSkipLoadingCosmeticBackgroundsInPIE = false;

	// List of cheats to auto-run during 'play in editor'
	UPROPERTY(config, EditAnywhere, Category=Lyra)
	TArray<FLyraCheatToRun> CheatsToRun;
	
	// Should messages broadcast through the gameplay message subsystem be logged?
	UPROPERTY(config, EditAnywhere, Category=GameplayMessages, meta=(ConsoleVariable="GameplayMessageSubsystem.LogMessages"))
	bool LogGameplayMessages = false;

#if WITH_EDITORONLY_DATA
	/** A list of common maps that will be accessible via the editor detoolbar */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Maps, meta=(AllowedClasses="/Script/Engine.World"))
	TArray<FSoftObjectPath> CommonEditorMaps;
#endif
	
#if WITH_EDITOR
public:
	// Called by the editor engine to let us pop reminder notifications when cheats are active
	D1GAME_API void OnPlayInEditorStarted() const;

private:
	void ApplySettings();
#endif

public:
	//~UObject interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostReloadConfig(FProperty* PropertyThatWasLoaded) override;
	virtual void PostInitProperties() override;
#endif
	//~End of UObject interface
};
