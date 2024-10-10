#include "D1MonsterAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Perception/AIPerceptionComponent.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MonsterAIController)

AD1MonsterAIController::AD1MonsterAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    bWantsPlayerState = true;
	bStopAILogicOnUnposses = false;
}

void AD1MonsterAIController::InitPlayerState()
{
	Super::InitPlayerState();

	BroadcastOnPlayerStateChanged();
}

void AD1MonsterAIController::CleanupPlayerState()
{
	Super::CleanupPlayerState();

	BroadcastOnPlayerStateChanged();
}

void AD1MonsterAIController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BroadcastOnPlayerStateChanged();
}

void AD1MonsterAIController::OnPlayerStateChanged()
{
	
}

void AD1MonsterAIController::OnUnPossess()
{
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}
	
	Super::OnUnPossess();
}

void AD1MonsterAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	
}

FGenericTeamId AD1MonsterAIController::GetGenericTeamId() const
{
	if (ID1TeamAgentInterface* PlayerStateTeamInterface = Cast<ID1TeamAgentInterface>(PlayerState))
	{
		return PlayerStateTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnD1TeamIndexChangedDelegate* AD1MonsterAIController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

ETeamAttitude::Type AD1MonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const ID1TeamAgentInterface* TeamAgent = Cast<ID1TeamAgentInterface>(OtherPawn->GetController()))
		{
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
			return OtherTeamID.GetId() != GetGenericTeamId().GetId() ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
		}
	}

	return ETeamAttitude::Neutral;
}

void AD1MonsterAIController::UpdateTeamAttitude(UAIPerceptionComponent* AIPerception)
{
	if (AIPerception)
	{
		AIPerception->RequestStimuliListenerUpdate();
	}
}

void AD1MonsterAIController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void AD1MonsterAIController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (LastSeenPlayerState)
	{
		if (ID1TeamAgentInterface* PlayerStateTeamInterface = Cast<ID1TeamAgentInterface>(LastSeenPlayerState))
		{
			OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetOnTeamIndexChangedDelegate()->RemoveAll(this);
		}
	}

	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (PlayerState)
	{
		if (ID1TeamAgentInterface* PlayerStateTeamInterface = Cast<ID1TeamAgentInterface>(PlayerState))
		{
			NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
		}
	}
	
	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	LastSeenPlayerState = PlayerState;
}
