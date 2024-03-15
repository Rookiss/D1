#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "System/D1AssetManager.h"
#include "Weapon/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ComboAttack)

UD1GameplayAbility_ComboAttack::UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ComboAttack::HandleTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (bBlocked)
		return;
	
	TArray<int32> AttackHitIndexes, BlockHitIndexes;

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

					if (Cast<AD1Character>(HitActor))
					{
						AttackHitIndexes.Add(i);
					}
					else if (Cast<AD1WeaponBase>(HitActor))
					{
						BlockHitIndexes.Add(i);
					}
					else
					{
						BlockHitIndexes.Add(i);
					}
				}
			}
		}
	}
	
	for (int32 BlockHitIndex : BlockHitIndexes)
	{
		const FHitResult& HitResult = *(InTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
		
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			
		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
		
		AbilitySystemComponent->Multicast_BlockAnimMontageForSeconds(BackwardMontage);
		
		UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
		FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
		{
			K2_EndAbility();
		});
		AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);
		
		bBlocked = true;
		return;
	}

	for (int32 AttackHitIndex : AttackHitIndexes)
	{
		const FHitResult& HitResult = *InTargetDataHandle.Data[AttackHitIndex]->GetHitResult();
		
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;

		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
		
		FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
		TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
	}
}

void UD1GameplayAbility_ComboAttack::TryContinueToNextStage()
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
