#include "D1TeamCreationComponent.h"

#include "GameModes/LyraExperienceManagerComponent.h"
#include "D1TeamPublicInfo.h"
#include "D1TeamPrivateInfo.h"
#include "D1TeamAgentInterface.h"
#include "Player/LyraPlayerState.h"
#include "Engine/World.h"
#include "GameModes/LyraGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamCreationComponent)

UD1TeamCreationComponent::UD1TeamCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PublicTeamInfoClass = AD1TeamPublicInfo::StaticClass();
	PrivateTeamInfoClass = AD1TeamPrivateInfo::StaticClass();
}

void UD1TeamCreationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UD1TeamCreationComponent::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerCreateTeams();
		ServerAssignPlayersToTeams();
	}
#endif
}

#if WITH_SERVER_CODE
void UD1TeamCreationComponent::ServerCreateTeams()
{
	for (const auto& KVP : TeamsToCreate)
	{
		const int32 TeamId = KVP.Key;
		ServerCreateTeam(TeamId, KVP.Value);
	}
}

void UD1TeamCreationComponent::ServerCreateTeam(int32 TeamID, UD1TeamDisplayAsset* DisplayAsset)
{
	check(HasAuthority());
	
	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AD1TeamPublicInfo* NewTeamPublicInfo = World->SpawnActor<AD1TeamPublicInfo>(PublicTeamInfoClass, SpawnInfo);
	checkf(NewTeamPublicInfo != nullptr, TEXT("Failed to create public team actor from class %s"), *GetPathNameSafe(*PublicTeamInfoClass));
	NewTeamPublicInfo->SetTeamID(TeamID);
	NewTeamPublicInfo->SetTeamDisplayAsset(DisplayAsset);

	AD1TeamPrivateInfo* NewTeamPrivateInfo = World->SpawnActor<AD1TeamPrivateInfo>(PrivateTeamInfoClass, SpawnInfo);
	checkf(NewTeamPrivateInfo != nullptr, TEXT("Failed to create private team actor from class %s"), *GetPathNameSafe(*PrivateTeamInfoClass));
	NewTeamPrivateInfo->SetTeamID(TeamID);
}

void UD1TeamCreationComponent::ServerAssignPlayersToTeams()
{
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ALyraPlayerState* LyraPS = Cast<ALyraPlayerState>(PS))
		{
			ServerChooseTeamForPlayer(LyraPS);
		}
	}
	
	ALyraGameMode* GameMode = Cast<ALyraGameMode>(GameState->AuthorityGameMode);
	check(GameMode);

	GameMode->OnGameModePlayerInitialized.AddUObject(this, &ThisClass::OnPlayerInitialized);
}

void UD1TeamCreationComponent::ServerChooseTeamForPlayer(ALyraPlayerState* PS)
{
	// TODO: TEMP
	if (PS->IsOnlyASpectator())
	{
		PS->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
	else
	{
		const FGenericTeamId TeamID = IntegerToGenericTeamID(DetermineTeamID());
		PS->SetGenericTeamId(TeamID);
	}
}

void UD1TeamCreationComponent::OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer)
{
	check(NewPlayer);
	check(NewPlayer->PlayerState);
	
	if (ALyraPlayerState* LyraPS = Cast<ALyraPlayerState>(NewPlayer->PlayerState))
	{
		ServerChooseTeamForPlayer(LyraPS);
	}
}

int32 UD1TeamCreationComponent::DetermineTeamID() const
{
	const int32 NumTeams = TeamsToCreate.Num();
	if (NumTeams > 0)
	{
		// Key: Team ID, Value: Number of players in the team
		TMap<int32, uint32> TeamMemberCountMap;
		TeamMemberCountMap.Reserve(NumTeams);

		for (const auto& KVP : TeamsToCreate)
		{
			const int32 TeamID = KVP.Key;
			TeamMemberCountMap.Add(TeamID, 0);
		}

		AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (ALyraPlayerState* LyraPS = Cast<ALyraPlayerState>(PS))
			{
				const int32 PlayerTeamID = LyraPS->GetTeamID();

				if ((PlayerTeamID != INDEX_NONE) && LyraPS->IsInactive() == false)
				{
					check(TeamMemberCountMap.Contains(PlayerTeamID))
					TeamMemberCountMap[PlayerTeamID] += 1;
				}
			}
		}
		
		int32 BestTeamID = INDEX_NONE;
		uint32 BestTeamMemberCount = TNumericLimits<uint32>::Max();
		
		for (const auto& KVP : TeamMemberCountMap)
		{
			const int32 TeamID = KVP.Key;
			if (TeamID == EnumToGenericTeamID(ED1TeamID::Monster))
				continue;
			
			const uint32 TeamMemberCount = KVP.Value;
			
			if (TeamMemberCount < BestTeamMemberCount)
			{
				BestTeamID = TeamID;
				BestTeamMemberCount = TeamMemberCount;
			}
			else if (TeamMemberCount == BestTeamMemberCount)
			{
				if ((TeamID < BestTeamID) || (BestTeamID == INDEX_NONE))
				{
					BestTeamID = TeamID;
					BestTeamMemberCount = TeamMemberCount;
				}
			}
		}

		return BestTeamID;
	}

	return INDEX_NONE;
}
#endif	// WITH_SERVER_CODE
