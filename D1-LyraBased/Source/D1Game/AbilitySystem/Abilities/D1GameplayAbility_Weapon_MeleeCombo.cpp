#include "D1GameplayAbility_Weapon_MeleeCombo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"
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

void UD1GameplayAbility_Weapon_MeleeCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			return;
		}
		
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
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

		TArray<int32> AttackHitIndexes;
		int32 BlockHitIndex = INDEX_NONE;

		for (int32 i = 0; i < LocalTargetDataHandle.Data.Num(); i++)
		{
			const TSharedPtr<FGameplayAbilityTargetData>& TargetData = LocalTargetDataHandle.Data[i];

			if (const FHitResult* HitResult = TargetData->GetHitResult())
			{
				if (AActor* HitActor = HitResult->GetActor())
				{
					if (HitActors.Contains(HitActor) == false)
					{
						HitActors.Add(HitActor);

						bool bHitSomething = false;
					
						if (HitActor->IsA(ALyraCharacter::StaticClass()))
						{
							bHitSomething = true;
							AttackHitIndexes.Add(i);
						}
						else if (AD1WeaponBase* HitWeapon = Cast<AD1WeaponBase>(HitActor))
						{
							if (HitWeapon->bCanBlock)
							{
								bHitSomething = true;
								BlockHitIndex = i;
							}
						}
						else
						{
							bHitSomething = true;
							BlockHitIndex = i;
						}

#if UE_EDITOR
						if (FORCE_DISABLE_DRAW_DEBUG == false && bHitSomething && bShowDebug)
						{
							FColor Color = (HasAuthority(&CurrentActivationInfo)) ? FColor::Red : FColor::Green;
							DrawDebugSphere(GetWorld(), HitResult->ImpactPoint, 4, 32, Color, false, 5);
						}
#endif // UE_EDITOR
					}
				}
			}
		}

		if (BlockHitIndex != INDEX_NONE)
		{
			const FHitResult& HitResult = *(LocalTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
			
			FGameplayCueParameters SourceCueParams;
			SourceCueParams.Location = HitResult.ImpactPoint;
			SourceCueParams.Normal = HitResult.ImpactNormal;
			SourceCueParams.PhysicalMaterial = HitResult.PhysMaterial;
			SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact_Weapon, SourceCueParams);

			SourceASC->BlockAnimMontageForSeconds(BackwardMontage);
			
			if (HasAuthority(&CurrentActivationInfo))
			{
				FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
				{
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				});
				UAnimInstance* AnimInstance = SourceASC->AbilityActorInfo->GetAnimInstance();
				AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);
			}
			
			bBlocked = true;
		}
		else
		{
			for (int32 AttackHitIndex : AttackHitIndexes)
			{
				const FHitResult& HitResult = *LocalTargetDataHandle.Data[AttackHitIndex]->GetHitResult();
				
				FGameplayCueParameters SourceCueParams;
				SourceCueParams.Location = HitResult.ImpactPoint;
				SourceCueParams.Normal = HitResult.ImpactNormal;
				SourceCueParams.PhysicalMaterial = HitResult.PhysMaterial;
				SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact_Weapon, SourceCueParams);

				if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitResult.GetActor()))
				{
					FGameplayCueParameters TargetCueParams;
					TargetCueParams.Location = HitResult.ImpactPoint;
					TargetASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_HitReact, TargetCueParams);
				}
				
				if (HasAuthority(&CurrentActivationInfo))
				{
					FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
					const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
					FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);
					EffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_PhysicalWeaponDamage, GetWeaponStatValue(D1GameplayTags::SetByCaller_PhysicalWeaponDamage));
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}
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
