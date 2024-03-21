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

void UD1GameplayAbility_Weapon_MeleeCombo::HandleTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (bBlocked)
		return;
	
	TArray<int32> AttackHitIndexes;
	int32 BlockHitIndex = -1;

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
					else if (AD1WeaponBase* HitWeaponActor = Cast<AD1WeaponBase>(HitActor))
					{
						FVector HitActorToPawn = (GetAvatarActorFromActorInfo()->GetActorLocation() - HitWeaponActor->GetActorLocation()).GetSafeNormal();
						float Dot = HitWeaponActor->GetActorForwardVector().Dot(HitActorToPawn);
						if (Dot >= 0.f)
						{
							BlockHitIndex = i;
							break;
						}
					}
					else
					{
						BlockHitIndex = i;
						break;
					}
				}
			}
		}
	}

	if (BlockHitIndex != -1)
	{
		const FHitResult& HitResult = *(InTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
		
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
			
		UD1AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		AbilitySystemComponent->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact, CueParameters);
		
		UAnimInstance* AnimInstance = AbilitySystemComponent->AbilityActorInfo->GetAnimInstance();
		AbilitySystemComponent->Multicast_BlockAnimMontageForSeconds(BackwardMontage);
		
		FOnMontageEnded MontageEnded = FOnMontageEnded::CreateWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
		{
			K2_EndAbility();
		});
		AnimInstance->Montage_SetEndDelegate(MontageEnded, BackwardMontage);
		
		bBlocked = true;
	}
	else
	{
		for (int32 AttackHitIndex : AttackHitIndexes)
		{
			const FHitResult& HitResult = *InTargetDataHandle.Data[AttackHitIndex]->GetHitResult();
			
			FVector Location = HitResult.ImpactPoint;
			if (HitResult.ImpactPoint.IsNearlyZero())
			{
				AD1Character* Character = Cast<AD1Character>(HitResult.GetActor());
				Location = Character->GetMesh()->GetBoneLocation(HitResult.BoneName);
			}

			DrawDebugSphere(GetWorld(), Location, 8, 12, FColor::Green, false, 2.f);
			
			FGameplayCueParameters CueParameters;
			CueParameters.Location = Location;
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
