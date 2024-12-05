#include "D1GameplayAbility_HitReact.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_HitReact)

UD1GameplayAbility_HitReact::UD1GameplayAbility_HitReact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	ActivationGroup = ELyraAbilityActivationGroup::Exclusive_Replaceable;
	bServerRespectsRemoteAbilityCancellation = true;
	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(D1GameplayTags::Ability_HitReact);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_HitReact);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = D1GameplayTags::GameplayEvent_HitReact;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UD1GameplayAbility_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (UAbilityTask_NetworkSyncPoint* NetSyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait))
	{
		NetSyncTask->OnSync.AddDynamic(this, &ThisClass::OnNetSync);
		NetSyncTask->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
		{
			if (APlayerCameraManager* PlayerCameraManager = LyraPlayerController->PlayerCameraManager)
			{
				PlayerCameraManager->StartCameraShake(HitReactCameraShakeClass);
			}
		}
	}
}

void UD1GameplayAbility_HitReact::OnNetSync()
{
	ALyraCharacter* TargetCharacter = GetLyraCharacterFromActorInfo();
	if (TargetCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UD1EquipManagerComponent* EquipManager = TargetCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	AD1EquipmentBase* EquippedActor = EquipManager->GetFirstEquippedActor();
	if (EquippedActor == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	const FHitResult* HitResultPtr = CurrentEventData.ContextHandle.GetHitResult();
	if (HitResultPtr == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	AActor* InstigatorActor = CurrentEventData.ContextHandle.GetInstigator();
	UAnimMontage* HitMontage = EquippedActor->GetHitMontage(InstigatorActor, HitResultPtr->ImpactPoint, HitResultPtr->bBlockingHit);
	
	if (UAbilityTask_PlayMontageAndWait* HitReactMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HitReactMontage"), HitMontage, 1.f, NAME_None, true))
	{
		HitReactMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		HitReactMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		HitReactMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		HitReactMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		HitReactMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_HitReact::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
