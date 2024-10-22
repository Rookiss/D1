#include "D1MonsterComponent.h"

#include "D1GameplayTags.h"
#include "D1LogChannels.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Misc/UObjectToken.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MonsterComponent)

const FName UD1MonsterComponent::NAME_ActorFeatureName("Monster");

UD1MonsterComponent::UD1MonsterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UD1MonsterComponent::OnRegister()
{
	Super::OnRegister();

	if (GetPawn<APawn>() == nullptr)
	{
		UE_LOG(LogD1, Error, TEXT("[UD1MonsterComponent::OnRegister] It MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("UD1MonsterComponent", "NotOnPawnError", "It MUST be placed on a Pawn Blueprint.");
			static const FName MonsterMessageLogName = TEXT("UD1MonsterComponent");
			
			FMessageLog(MonsterMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(MonsterMessageLogName).Open();
		}
#endif
	}
	else
	{
		RegisterInitStateFeature();
	}
}

void UD1MonsterComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(ULyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	
	ensure(TryToChangeInitState(D1GameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UD1MonsterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UD1MonsterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == D1GameplayTags::InitState_DataAvailable && DesiredState == D1GameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALyraPlayerState* LyraPlayerState = GetPlayerState<ALyraPlayerState>();
		if (ensure(Pawn && LyraPlayerState) == false)
			return;
		
		if (ULyraPawnExtensionComponent* PawnExtensionComponent = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtensionComponent->InitializeAbilitySystem(LyraPlayerState->GetLyraAbilitySystemComponent(), LyraPlayerState);
		}
	}
}

void UD1MonsterComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == ULyraPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == D1GameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UD1MonsterComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { D1GameplayTags::InitState_Spawned, D1GameplayTags::InitState_DataAvailable, D1GameplayTags::InitState_DataInitialized, D1GameplayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

bool UD1MonsterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (CurrentState.IsValid() == false && DesiredState == D1GameplayTags::InitState_Spawned)
	{
		if (Pawn)
			return true;
	}
	else if (CurrentState == D1GameplayTags::InitState_Spawned && DesiredState == D1GameplayTags::InitState_DataAvailable)
	{
		if (GetPlayerState<ALyraPlayerState>() == nullptr)
			return false;

		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller) && (Controller->PlayerState) && (Controller->PlayerState->GetOwner() == Controller);
			if (bHasControllerPairedWithPS == false)
				return false;
		}

		return true;
	}
	else if (CurrentState == D1GameplayTags::InitState_DataAvailable && DesiredState == D1GameplayTags::InitState_DataInitialized)
	{
		ALyraPlayerState* LyraPlayerState = GetPlayerState<ALyraPlayerState>();
		return LyraPlayerState && Manager->HasFeatureReachedInitState(Pawn, ULyraPawnExtensionComponent::NAME_ActorFeatureName, D1GameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == D1GameplayTags::InitState_DataInitialized && DesiredState == D1GameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}
