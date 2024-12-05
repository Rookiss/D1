#include "D1GameplayAbility_Skill_ShieldBash.h"

#include "D1GameplayTags.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
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
	bServerRespectsRemoteAbilityCancellation = true;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	AbilityTags.AddTag(D1GameplayTags::Ability_Attack_Skill_1);
	BlockAbilitiesWithTag.AddTag(D1GameplayTags::Ability_Jump);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_RejectHitReact);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Skill);
	
	FD1EquipmentInfo EquipmentInfo;
	EquipmentInfo.WeaponHandType = EWeaponHandType::LeftHand;
	EquipmentInfo.RequiredWeaponType = EWeaponType::Shield;
	EquipmentInfos.Add(EquipmentInfo);
}

void UD1GameplayAbility_Skill_ShieldBash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr || LyraCharacter->GetCharacterMovement()->IsFalling())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	SetCameraMode(CameraModeClass);

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreLookInput(true);
	}

	if (UAbilityTask_PlayMontageAndWait* ShieldBashMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ShieldBashMontage"), ShieldBashMontage, 1.f, NAME_None, true))
	{
		ShieldBashMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShieldBashMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShieldBashMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShieldBashMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShieldBashMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* ShieldBashBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		ShieldBashBeginTask->EventReceived.AddDynamic(this, &ThisClass::OnShieldBashBegin);
		ShieldBashBeginTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_ShieldBash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreLookInput(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Skill_ShieldBash::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Skill_ShieldBash::OnShieldBashBegin(FGameplayEventData Payload)
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
