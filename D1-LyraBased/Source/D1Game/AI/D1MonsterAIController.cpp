#include "D1MonsterAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "D1Define.h"
#include "D1LogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MonsterAIController)

AD1MonsterAIController::AD1MonsterAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
    bWantsPlayerState = true;
	bStopAILogicOnUnposses = false;

	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfigSight"));
	AISenseConfigSight->SightRadius = 200.f;
	AISenseConfigSight->LoseSightRadius = 500.f;
	AISenseConfigSight->PeripheralVisionAngleDegrees = 60.f;
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals = false;

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
}

void AD1MonsterAIController::InitPlayerState()
{
	Super::InitPlayerState();

	BroadcastOnPlayerStateChanged();

	if (HasAuthority())
	{
		if (ALyraPlayerState* LyraPS = Cast<ALyraPlayerState>(PlayerState))
		{
			LyraPS->SetGenericTeamId(EnumToGenericTeamID(ED1TeamID::Monster));
		}
	}
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

void AD1MonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UCrowdFollowingComponent* CrowdFollowingComponent = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdFollowingComponent->SetCrowdSimulationState(ECrowdSimulationState::Enabled);
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
		CrowdFollowingComponent->SetAvoidanceGroup(1);
		CrowdFollowingComponent->SetGroupsToAvoid(1);
		CrowdFollowingComponent->SetCrowdCollisionQueryRange(CrowdCollisionQueryRange);
	}
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
	UE_LOG(LogD1Team, Error, TEXT("You can't set the team ID on a ai controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
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

void AD1MonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor && Stimulus.WasSuccessfullySensed())
	{
		if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
		}
	}
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
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamID(OldTeam), IntegerToGenericTeamID(NewTeam));
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
