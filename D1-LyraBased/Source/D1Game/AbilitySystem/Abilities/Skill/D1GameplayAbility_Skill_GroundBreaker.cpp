#include "D1GameplayAbility_Skill_GroundBreaker.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/LyraCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_GroundBreaker)

UD1GameplayAbility_Skill_GroundBreaker::UD1GameplayAbility_Skill_GroundBreaker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_Attack_Skill_1);
	BlockAbilitiesWithTag.AddTag(D1GameplayTags::Ability_Jump);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_RejectHitReact);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Skill);

	FD1EquipmentInfo EquipmentInfo;
	EquipmentInfo.WeaponHandType = EWeaponHandType::TwoHand;
	EquipmentInfo.RequiredWeaponType = EWeaponType::GreatSword;
	EquipmentInfos.Add(EquipmentInfo);
}

void UD1GameplayAbility_Skill_GroundBreaker::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (UAbilityTask_PlayMontageAndWait* GroundBreakerMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("GroundBreakerMontage"), GroundBreakerMontage, 1.f, NAME_None, true))
	{
		GroundBreakerMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		GroundBreakerMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		GroundBreakerMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		GroundBreakerMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		GroundBreakerMontageTask->ReadyForActivation();
	}
	
	if (UAbilityTask_WaitGameplayEvent* GroundBreakerBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		GroundBreakerBeginTask->EventReceived.AddDynamic(this, &ThisClass::OnGroundBreakerBegin);
		GroundBreakerBeginTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Skill_GroundBreaker::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();
	
	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreLookInput(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Skill_GroundBreaker::OnGroundBreakerBegin(FGameplayEventData Payload)
{
	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreLookInput(true);
	}
	
	ExecuteGroundBreaker();
}

void UD1GameplayAbility_Skill_GroundBreaker::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Skill_GroundBreaker::ExecuteGroundBreaker()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;

	AD1EquipmentBase* EquipmentActor = GetFirstEquipmentActor();
	check(EquipmentActor);
	
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	float ScaledCapsuleRadius = LyraCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float ScaledCapsuleHalfHeight = LyraCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector Start = LyraCharacter->GetActorLocation() + (LyraCharacter->GetActorForwardVector() * DistanceOffset);
	FVector End = Start;
	FVector HalfSize = FVector(ScaledCapsuleRadius * 3.f, ScaledCapsuleRadius * 3.f, ScaledCapsuleHalfHeight);
	FRotator Orientation = UKismetMathLibrary::MakeRotFromX(LyraCharacter->GetActorForwardVector());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore = { LyraCharacter };
	
	TArray<FHitResult> OutHitResults;
	TMap<ALyraCharacter*, TPair<int32/*HitResult Index*/, float/*DistanceSq*/>> HitInfos;
	
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), Start, End, HalfSize, Orientation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHitResults, true))
	{
		for (int32 i = 0; i < OutHitResults.Num(); i++)
		{
			const FHitResult& HitResult = OutHitResults[i];
			
			ALyraCharacter* HitCharacter = Cast<ALyraCharacter>(HitResult.GetActor());
			if (HitCharacter == nullptr)
				continue;

			float DistanceSq = FVector::DistSquared(LyraCharacter->GetActorLocation(), HitResult.ImpactPoint);
			
			if (TPair<int32, float>* HitInfo = HitInfos.Find(HitCharacter))
			{
				float PrevDistanceSq = (*HitInfo).Value;
				if (DistanceSq < PrevDistanceSq)
				{
					(*HitInfo).Key = i;
					(*HitInfo).Value = DistanceSq;
				}
			}
			else
			{
				HitInfos.Add(HitCharacter, {i, DistanceSq});
			}
		}

		for (const auto& HitInfo : HitInfos)
		{
			const int32 HitResultIndex = HitInfo.Value.Key;
			const FHitResult& HitResult = OutHitResults[HitResultIndex];
			ProcessHitResult(HitResult, Damage, IsCharacterBlockingHit(Cast<ALyraCharacter>(HitResult.GetActor())), nullptr, EquipmentActor);

			FGameplayEventData EventData;
			EventData.EventMagnitude = StunDruation;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), D1GameplayTags::GameplayEvent_Stun, EventData);
		}
	}
}
