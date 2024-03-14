#include "D1GameplayAbility_ComboAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapon/D1WeaponBase.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/D1AssetManager.h"

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
					
					if (HitActor->IsA(AD1Character::StaticClass()))
					{
						AttackHitIndexes.Add(i);
					}
					else
					{
						if (AD1WeaponBase* WeaponBase = Cast<AD1WeaponBase>(HitResult->GetActor()))
						{
							if (WeaponBase->bCanBlock)
							{
								float Degree = UKismetMathLibrary::DegAcos((HitResult->ImpactPoint - WeaponBase->WeaponMesh->GetComponentLocation()).GetSafeNormal().Dot(WeaponBase->WeaponMesh->GetForwardVector())); 
								if (Degree <= 90.f)
								{
									BlockHitIndexes.Add(i);
								}
							}
						}
						else
						{
							BlockHitIndexes.Add(i);
						}
					}
				}
			}
		}
	}
	
	for (int32 BlockHitIndex : BlockHitIndexes)
	{
		const FHitResult& HitResult = *InTargetDataHandle.Data[BlockHitIndex]->GetHitResult();
		
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			
		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
		
		UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
		float EffectivePlayRate = AnimInstance->Montage_GetEffectivePlayRate(CurrentMontage);
		float Position = AnimInstance->Montage_GetPosition(CurrentMontage);

		if (HasAuthority(&CurrentActivationInfo))
		{
			AnimInstance->Montage_PlayWithBlendSettings(CurrentMontage, FMontageBlendSettings(0.f), -EffectivePlayRate / 3.f, EMontagePlayReturnType::MontageLength, Position);
			GetWorld()->GetTimerManager().SetTimer(BlockMontageTimerHandle, this, &ThisClass::HandleBlockMontage, 0.25f, false);
		}
		
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
		
		if (HasAuthority(&CurrentActivationInfo))
		{
			FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
			TSubclassOf<UGameplayEffect> DamageEffectClass = UD1AssetManager::GetSubclassByName<UGameplayEffect>("GE_Damage");
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
			EffectSpecHandle.Data->AddDynamicAssetTag(D1GameplayTags::Attack_Physical);
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				
			AbilitySystemComponent->Multicast_SlowAnimMontageForSeconds(AbilitySystemComponent->GetCurrentActiveMontage(), 0.2f, 0.1f);
		}
	}
}

void UD1GameplayAbility_ComboAttack::HandleBlockMontage()
{
	UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
	AnimInstance->Montage_Stop(0.2f, CurrentMontage);
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
