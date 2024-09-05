#include "D1GameplayAbility_Weapon_MeleeCombo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_MeleeCombo)

UD1GameplayAbility_Weapon_MeleeCombo::UD1GameplayAbility_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1GameplayAbility_Weapon_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	HitActors.Reset();
	bInputPressed = false;
	bInputReleased = false;
	bBlocked = false;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_MeleeCombo::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (bBlocked)
		return;
	
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	check(SourceASC);

	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow	ScopedPrediction(SourceASC, GetCurrentActivationInfo().GetActivationPredictionKey());
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InTargetDataHandle)));

		TArray<int32> CharacterHitIndexes;
		TArray<int32> BlockHitIndexes;
		ParseTargetData(LocalTargetDataHandle, CharacterHitIndexes, BlockHitIndexes);

		if (BlockHitIndexes.Num() > 0)
		{
			FHitResult HitResult = *(LocalTargetDataHandle.Data[BlockHitIndexes[0]]->GetHitResult());
			
			FGameplayCueParameters SourceCueParams;
			SourceCueParams.Location = HitResult.ImpactPoint;
			SourceCueParams.Normal = HitResult.ImpactNormal;
			SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Weapon_Impact, SourceCueParams);
			
			SourceASC->BlockAnimMontageForSeconds(BackwardMontage);
			
			if (HasAuthority(&CurrentActivationInfo))
			{
				FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
				{
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				});
				UAnimInstance* AnimInstance = SourceASC->AbilityActorInfo->GetAnimInstance();
				AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);

				FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
				const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);

				FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
				HitResult.bBlockingHit = true;
				EffectContextHandle.AddHitResult(HitResult);
				EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), WeaponActor);
				EffectSpecHandle.Data->SetContext(EffectContextHandle);
					
				EffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseDamage, GetWeaponStatValue(D1GameplayTags::SetByCaller_BaseDamage) / 3.f);
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}
			
			bBlocked = true;
			DrawDebugHitPoint(HitResult);
		}
		else
		{
			for (int32 CharqacterHitIndex : CharacterHitIndexes)
			{
				FHitResult HitResult = *LocalTargetDataHandle.Data[CharqacterHitIndex]->GetHitResult();
				ProcessHitResult(HitResult, Damage, false, nullptr);
			}
		}
	}
}

void UD1GameplayAbility_Weapon_MeleeCombo::TryContinueToNextStage()
{
	bool bCanContinue = NextAbilityClass && (bInputPressed || bInputReleased == false) && (bBlocked == false);
	if (bCanContinue)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
		{
			AbilitySystemComponent->TryActivateAbilityByClass(NextAbilityClass);
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else if (bBlocked == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
