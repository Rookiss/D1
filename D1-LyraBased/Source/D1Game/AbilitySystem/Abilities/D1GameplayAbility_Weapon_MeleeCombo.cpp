#include "D1GameplayAbility_Weapon_MeleeCombo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_MeleeCombo)

UD1GameplayAbility_Weapon_MeleeCombo::UD1GameplayAbility_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1GameplayAbility_Weapon_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	HitActors.Reset();
	bInputReleased = false;
	bBlocked = false;

	UAbilitySystemComponent* AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(AbilitySystemComponent);

	OnTargetDataReadyCallbackDelegateHandle = AbilitySystemComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);
	
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

		UAbilitySystemComponent* AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent);
		
		AbilitySystemComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
		AbilitySystemComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UD1GameplayAbility_Weapon_MeleeCombo::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, FGameplayTag ApplicationTag)
{
	if (bBlocked)
		return;

	// TODO: https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking
	// TODO: https://ftp.cs.wpi.edu/pub/techreports/pdf/21-06.pdf
	// Incoming Delay + Time Warp

	ULyraAbilitySystemComponent* AbilitySystemComponent = GetLyraAbilitySystemComponentFromActorInfo();
	check(AbilitySystemComponent);

	if (AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow	ScopedPrediction(AbilitySystemComponent);

		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InTargetDataHandle)));
	
		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && CurrentActorInfo->IsNetAuthority() == false;
		if (bShouldNotifyServer)
		{
			AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}

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

#if ENABLE_DRAW_DEBUG
						if (bHitSomething && bShowDebug)
						{
							FColor Color = (HasAuthority(&CurrentActivationInfo)) ? FColor::Red : FColor::Green;
							DrawDebugSphere(GetWorld(), HitResult->ImpactPoint, 4, 32, Color, false, 5);
						}
#endif // ENABLE_DRAW_DEBUG
					}
				}
			}
		}

		if (BlockHitIndex != INDEX_NONE)
		{
			const FHitResult& HitResult = *(LocalTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
			
			FGameplayCueParameters CueParameters;
			CueParameters.Location = HitResult.ImpactPoint;
			CueParameters.Normal = HitResult.ImpactNormal;
			CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact_Weapon, CueParameters);

			AbilitySystemComponent->BlockAnimMontageForSeconds(BackwardMontage);
			
			if (HasAuthority(&CurrentActivationInfo))
			{
				FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
				{
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				});
				UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
				AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);
			}
			
			bBlocked = true;
		}
		else
		{
			for (int32 AttackHitIndex : AttackHitIndexes)
			{
				const FHitResult& HitResult = *LocalTargetDataHandle.Data[AttackHitIndex]->GetHitResult();
				
				FGameplayCueParameters CueParameters;
				CueParameters.Location = HitResult.ImpactPoint;
				CueParameters.Normal = HitResult.ImpactNormal;
				CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
				AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact_Weapon, CueParameters);
				
				if (HasAuthority(&CurrentActivationInfo))
				{
					FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
					FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffectClass);
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}
			}
		}
	}

	AbilitySystemComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UD1GameplayAbility_Weapon_MeleeCombo::TryContinueToNextStage()
{
	bool bCanContinue = NextAbilityClass && (bInputReleased == false) && (bBlocked == false);
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
