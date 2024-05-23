// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGameState.h"

#include "D1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "Messages/LyraVerbMessage.h"
#include "Player/LyraPlayerState.h"
#include "D1LogChannels.h"
#include "AbilitySystem/Phases/LyraGamePhaseSubsystem.h"
#include "Messages/LyraNotificationMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameState)

class APlayerState;
class FLifetimeProperty;

extern ENGINE_API float GAverageFPS;

ALyraGameState::ALyraGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<ULyraExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
	
	ServerFPS = 0.0f;

	NextBattlePlayers.SetNumZeroed(2);
}

void ALyraGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALyraGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* ALyraGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALyraGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALyraGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (OnPlayerStatesChanged.IsBound())
	{
		OnPlayerStatesChanged.Broadcast();
	}
}

void ALyraGameState::RemovePlayerState(APlayerState* PlayerState)
{
	CancelBattlePlayer_Internal(PlayerState);
	
	Super::RemovePlayerState(PlayerState);

	if (OnPlayerStatesChanged.IsBound())
	{
		OnPlayerStatesChanged.Broadcast();
	}
}

void ALyraGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{
	// Remove inactive and bots
	for (int32 i = PlayerArray.Num() - 1; i >= 0; i--)
	{
		APlayerState* PlayerState = PlayerArray[i];
		if (PlayerState && (PlayerState->IsABot() || PlayerState->IsInactive()))
		{
			RemovePlayerState(PlayerState);
		}
	}
}

void ALyraGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
	DOREPLIFETIME(ThisClass, NextBattlePlayers);
	DOREPLIFETIME(ThisClass, BettingCoins);
	DOREPLIFETIME_CONDITION(ThisClass, RecorderPlayerState, COND_ReplayOnly);
}

void ALyraGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

void ALyraGameState::MulticastMessageToClients_Implementation(const FLyraVerbMessage Message)
{
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

void ALyraGameState::MulticastReliableMessageToClients_Implementation(const FLyraVerbMessage Message)
{
	MulticastMessageToClients_Implementation(Message);
}

float ALyraGameState::GetServerFPS() const
{
	return ServerFPS;
}

void ALyraGameState::SetRecorderPlayerState(APlayerState* NewPlayerState)
{
	if (RecorderPlayerState == nullptr)
	{
		// Set it and call the rep callback so it can do any record-time setup
		RecorderPlayerState = NewPlayerState;
		OnRep_RecorderPlayerState();
	}
	else
	{
		UE_LOG(LogD1, Warning, TEXT("SetRecorderPlayerState was called on %s but should only be called once per game on the primary user"), *GetName());
	}
}

APlayerState* ALyraGameState::GetRecorderPlayerState() const
{
	// TODO: Maybe auto select it if null?

	return RecorderPlayerState;
}

void ALyraGameState::OnRep_RecorderPlayerState()
{
	OnRecorderPlayerStateChangedEvent.Broadcast(RecorderPlayerState);
}

void ALyraGameState::PollBattlePlayers()
{
	if (HasAuthority() == false)
		return;
	
	if (QueuedBattlePlayers.Num() > 0 && NextBattlePlayers[0] == nullptr)
	{
		NextBattlePlayers[0] = QueuedBattlePlayers[0];
		QueuedBattlePlayers.RemoveAtSwap(0);
	}

	if (QueuedBattlePlayers.Num() > 0 && NextBattlePlayers[1] == nullptr)
	{
		int32 RandIndex = FMath::RandRange(0, QueuedBattlePlayers.Num() - 1);
		NextBattlePlayers[1] = QueuedBattlePlayers[RandIndex];
		QueuedBattlePlayers.RemoveAtSwap(RandIndex);
	}
}

bool ALyraGameState::TryApplyBattlePlayer(APlayerState* PlayerState)
{
	if (HasAuthority() == false)
		return false;

	if (IsAppliedBattlePlayer(PlayerState))
		return false;

	for (int32 i = 0; i < NextBattlePlayers.Num(); i++)
	{
		if (NextBattlePlayers[i])
			continue;

		NextBattlePlayers[i] = PlayerState;
		return true;
	}
	
	QueuedBattlePlayers.Add(PlayerState);
	return true;
}

bool ALyraGameState::TryCancelBattlePlayer(APlayerState* PlayerState)
{
	if (HasAuthority() == false)
		return false;

	ULyraGamePhaseSubsystem* GamePhaseSubsystem = GetWorld()->GetSubsystem<ULyraGamePhaseSubsystem>();
	if (GamePhaseSubsystem->IsPhaseActive(D1GameplayTags::GamePhase_WaitForPlayers) == false && NextBattlePlayers.Contains(PlayerState))
		return false;
	
	return CancelBattlePlayer_Internal(PlayerState);
}

bool ALyraGameState::IsAppliedBattlePlayer(APlayerState* PlayerState)
{
	if (PlayerState == nullptr)
		return false;
	
	if (NextBattlePlayers.Contains(PlayerState) || QueuedBattlePlayers.Contains(PlayerState))
		return true;
	
	return false;
}

bool ALyraGameState::IsNextBattlePlayer(APlayerState* PlayerState)
{
	if (PlayerState == nullptr)
		return false;

	if (NextBattlePlayers.Contains(PlayerState))
		return true;

	return false;
}

bool ALyraGameState::IsQueuedBattlePlayer(APlayerState* PlayerState)
{
	if (PlayerState == nullptr)
		return false;
	
	if (QueuedBattlePlayers.Contains(PlayerState))
		return true;
	
	return false;
}

bool ALyraGameState::CancelBattlePlayer_Internal(APlayerState* PlayerState)
{
	if (HasAuthority() == false)
		return false;
	
	for (int32 i = 0; i < NextBattlePlayers.Num(); i++)
	{
		if (NextBattlePlayers[i] != PlayerState)
			continue;

		NextBattlePlayers[i] = nullptr;
		return true;
	}

	return QueuedBattlePlayers.Remove(PlayerState) > 0;
}

bool ALyraGameState::TryApplyBettingCoins(FCoinApplyRequest CoinApplyRequest, APlayerState* PlayerState)
{
	if (HasAuthority() == false)
		return false;
	
	ALyraPlayerState* LyraPlayerState = Cast<ALyraPlayerState>(PlayerState);
	if (LyraPlayerState == nullptr)
		return false;

	ULyraGamePhaseSubsystem* GamePhaseSubsystem = GetWorld()->GetSubsystem<ULyraGamePhaseSubsystem>();
	if (GamePhaseSubsystem->IsPhaseActive(D1GameplayTags::GamePhase_Betting) == false)
		return false;
	
	if (CoinApplyRequest.TeamID == 0 || CoinApplyRequest.Coin <= 0)
		return false;

	if (CoinApplyRequest.Coin > LyraPlayerState->Coin)
		return false;

	if (IsAppiedBettingCoins(PlayerState))
		return false;

	LyraPlayerState->Coin -= CoinApplyRequest.Coin;
	LyraPlayerState->SetGenericTeamId(CoinApplyRequest.TeamID);
	
	BettingCoins.Add(FCoinApplyEntry{ LyraPlayerState, CoinApplyRequest.Coin });
	
	return true;
}

bool ALyraGameState::TryCancelBettingCoins(APlayerState* PlayerState)
{
	if (HasAuthority() == false)
		return false;
	
	ALyraPlayerState* LyraPlayerState = Cast<ALyraPlayerState>(PlayerState);
	if (LyraPlayerState == nullptr)
		return false;

	ULyraGamePhaseSubsystem* GamePhaseSubsystem = GetWorld()->GetSubsystem<ULyraGamePhaseSubsystem>();
	if (GamePhaseSubsystem->IsPhaseActive(D1GameplayTags::GamePhase_Betting) == false)
		return false;

	int32 Index = INDEX_NONE;
	
	for (int32 i = 0; i < BettingCoins.Num(); i++)
	{
		if (BettingCoins[i].PlayerState == PlayerState)
		{
			Index = i;
			break;
		}
	}

	if (Index == INDEX_NONE)
		return false;
	
	LyraPlayerState->Coin += BettingCoins[Index].Coin;
	LyraPlayerState->SetGenericTeamId(0);

	BettingCoins.RemoveAtSwap(Index);
	return true;
}

bool ALyraGameState::IsAppiedBettingCoins(APlayerState* PlayerState)
{
	for (FCoinApplyEntry& BettingCoin : BettingCoins)
	{
		if (BettingCoin.PlayerState == PlayerState)
			return true;
	}
	return false;
}

FCoinApplyEntry ALyraGameState::GetAppiedBettingCoins(APlayerState* PlayerState)
{
	for (FCoinApplyEntry& BettingCoin : BettingCoins)
	{
		if (BettingCoin.PlayerState == PlayerState)
			return BettingCoin;
	}
	return FCoinApplyEntry();
}

void ALyraGameState::OnRep_NextBattlePlayers()
{
	FLyraVerbMessage VerbMessage;
	VerbMessage.Verb = D1GameplayTags::Message_NextBattlePlayersChanged;
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(VerbMessage.Verb, VerbMessage);
}

void ALyraGameState::OnRep_BettingCoins()
{
	FLyraVerbMessage VerbMessage;
	VerbMessage.Verb = D1GameplayTags::Message_BettingCoinsChanged;
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(VerbMessage.Verb, VerbMessage);
}
