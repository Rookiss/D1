#pragma once

#include "Components/GameStateComponent.h"
#include "D1TeamCreationComponent.generated.h"

class ULyraExperienceDefinition;
class AD1TeamPublicInfo;
class AD1TeamPrivateInfo;
class ALyraPlayerState;
class AGameModeBase;
class APlayerController;
class UD1TeamDisplayAsset;

UCLASS(Blueprintable)
class UD1TeamCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UD1TeamCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

private:
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

protected:
	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TMap<uint8, TObjectPtr<UD1TeamDisplayAsset>> TeamsToCreate;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<AD1TeamPublicInfo> PublicTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Category=Teams)
	TSubclassOf<AD1TeamPrivateInfo> PrivateTeamInfoClass;

#if WITH_SERVER_CODE
protected:
	virtual void ServerCreateTeams();
	virtual void ServerAssignPlayersToTeams();
	virtual void ServerChooseTeamForPlayer(ALyraPlayerState* PS);

private:
	void OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer);
	void ServerCreateTeam(int32 TeamID, UD1TeamDisplayAsset* DisplayAsset);
	
	int32 DetermineTeamID() const;

	uint32 MaxTeamMemberCount = 3;
#endif
};
