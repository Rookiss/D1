#include "D1GameplayAbility_Weapon_MeleeCombo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "System/D1AssetManager.h"
#include "Weapon/D1WeaponBase.h"

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
	bAttacked = false;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Weapon_MeleeCombo::HandleTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (bBlocked)
		return;

	TArray<int32> AttackHitIndexes;
	int32 BlockHitIndex = INDEX_NONE;

	for (int32 i = 0; i < InTargetDataHandle.Data.Num(); i++)
	{
		const TSharedPtr<FGameplayAbilityTargetData>& TargetData = InTargetDataHandle.Data[i];

		if (const FHitResult* HitResult = TargetData->GetHitResult())
		{
			if (AActor* HitActor = HitResult->GetActor())
			{
				if (HitActors.Contains(HitActor) == false)
				{
					HitActors.Add(HitActor);

					bool bHitSomething = false;
					
					if (HitActor->IsA(AD1Character::StaticClass()))
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

					if (bHitSomething)
					{
						FColor Color = (HasAuthority(&CurrentActivationInfo)) ? FColor::Red : FColor::Green;
						DrawDebugSphere(GetWorld(), HitResult->ImpactPoint, 4, 32, Color, false, 5);
					}
					
					if (BlockHitIndex != INDEX_NONE)
						break;
				}
			}
		}
	}

	// TODO: https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking
	// TODO: https://ftp.cs.wpi.edu/pub/techreports/pdf/21-06.pdf
	
	FScopedPredictionWindow ScopedPrediction(GetAbilitySystemComponent(), CurrentActivationInfo.GetActivationPredictionKey());
	
	if (BlockHitIndex != -1)
	{
		const FHitResult& HitResult = *(InTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
		
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			
		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);

		if (HasAuthority(&CurrentActivationInfo))
		{
			AbilitySystemComponent->Multicast_BlockAnimMontageForSeconds(BackwardMontage);
			FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
			{
				K2_EndAbility();
			});
			UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
			AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);
		}
		else
		{
			AbilitySystemComponent->BlockAnimMontageForSeconds(BackwardMontage);
		}
		bBlocked = true;
	}
	else
	{
		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		
		for (int32 AttackHitIndex : AttackHitIndexes)
		{
			const FHitResult& HitResult = *InTargetDataHandle.Data[AttackHitIndex]->GetHitResult();
			
			FGameplayCueParameters CueParameters;
			CueParameters.Location = HitResult.ImpactPoint;
			CueParameters.Normal = HitResult.ImpactNormal;
			CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			
			AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);

			if (HasAuthority(&CurrentActivationInfo))
			{
				FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
				TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
				EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}
		}

		if (AttackHitIndexes.Num() > 0)
		{
			if (bAttacked == false)
			{
				if (HasAuthority(&CurrentActivationInfo))
				{
					AbilitySystemComponent->Multicast_SlowAnimMontageForSeconds(GetCurrentMontage());
				}
				else
				{
					AbilitySystemComponent->SlowAnimMontageForSeconds(GetCurrentMontage());
				}
			}
			
			bAttacked = true;
		}
	}
}

void UD1GameplayAbility_Weapon_MeleeCombo::TryContinueToNextStage()
{
	bool bCanContinue = (NextStageTag.IsValid()) && (bInputReleased == false) && (bBlocked == false);
	if (bCanContinue)
	{
		HitActors.Reset();
		bInputReleased = false;
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(NextStageTag.GetSingleTagContainer());
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
