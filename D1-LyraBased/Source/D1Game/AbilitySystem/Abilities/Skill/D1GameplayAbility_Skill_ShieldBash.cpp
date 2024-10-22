#include "D1GameplayAbility_Skill_ShieldBash.h"

#include "D1GameplayTags.h"
#include "D1LogChannels.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Camera/LyraCameraMode.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/LyraPlayerController.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_ShieldBash)

UD1GameplayAbility_Skill_ShieldBash::UD1GameplayAbility_Skill_ShieldBash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Skill_ShieldBash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ALyraCharacter* SourceCharacter = GetLyraCharacterFromActorInfo();
	if (SourceCharacter == nullptr || SourceCharacter->GetCharacterMovement()->IsFalling() || K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

	if (CameraModeClass)
	{
		SetCameraMode(CameraModeClass);
	}

	if (ALyraPlayerController* SourcePlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		SourcePlayerController->SetIgnoreLookInput(true);
	}

	if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ShieldBash"), ShieldBashMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageBegin);
		WaitGameplayEventTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_ShieldBash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();

	if (ALyraPlayerController* SourcePlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		SourcePlayerController->SetIgnoreLookInput(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Skill_ShieldBash::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Skill_ShieldBash::OnMontageBegin(FGameplayEventData Payload)
{
	ALyraCharacter* SourceCharacter = GetLyraCharacterFromActorInfo();
	if (SourceCharacter == nullptr)
		return;
	
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	FVector CapsulePosition = SourceCharacter->GetActorLocation() + (SourceCharacter->GetActorForwardVector() * Distance);
	float Radius = SourceCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() * RadiusMultiplier;
	float HalfHeight = SourceCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore = { SourceCharacter };

	TArray<AActor*> OverlappedActors;
	if (UKismetSystemLibrary::CapsuleOverlapActors(this, CapsulePosition, Radius, HalfHeight, ObjectTypeQueries, ALyraCharacter::StaticClass(), ActorsToIgnore, OverlappedActors) == false)
		return;
	
	for (AActor* OverlappedActor : OverlappedActors)
	{
		ALyraCharacter* TargetCharacter = Cast<ALyraCharacter>(OverlappedActor);
		if (TargetCharacter == nullptr)
			continue;
		
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
		if (TargetASC == nullptr)
			continue;

		TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseDamage, Damage);
		
		FHitResult HitResult;
		HitResult.ImpactPoint = OverlappedActor->GetActorLocation();
			
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.AddHitResult(HitResult);
		EffectSpecHandle.Data->SetContext(EffectContextHandle);
		
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

		FGameplayEventData EventPayload;
		EventPayload.Instigator = SourceCharacter;
		EventPayload.EventMagnitude = StunDuration;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetCharacter, D1GameplayTags::GameplayEvent_Knockback, EventPayload);
	}
}
