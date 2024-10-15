#include "D1GameplayAbility_Skill_Targeting.h"

#include "AbilitySystemComponent.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_Targeting)

UD1GameplayAbility_Skill_Targeting::UD1GameplayAbility_Skill_Targeting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Skill_Targeting::ApplyTarget()
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;

				for (TSubclassOf<UGameplayEffect> GameplayEffectClass : GameplayEffectClasses)
				{
					if (GameplayEffectClass)
					{
						FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffectClass);
						FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
						EffectContextHandle.AddHitResult(HitResult);
						EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), WeaponActor);
						EffectSpecHandle.Data->SetContext(EffectContextHandle);
						ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
					}
				}

				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
				if (TargetASC && BurstGameplayCueTag.IsValid())
				{
					FGameplayCueParameters GameplayCueParams;

					if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(HitResult.GetActor()))
					{
						GameplayCueParams.TargetAttachComponent = LyraCharacter->GetMesh();
					}
					else
					{
						GameplayCueParams.TargetAttachComponent = HitResult.GetActor()->GetRootComponent();
					}
					
					GameplayCueParams.Location = HitResult.ImpactPoint;
					GameplayCueParams.Normal = HitResult.ImpactNormal;
					GameplayCueParams.PhysicalMaterial = HitResult.PhysMaterial;
					TargetASC->ExecuteGameplayCue(BurstGameplayCueTag, GameplayCueParams);
				}
			}
		}
	}

	TargetDataHandle.Clear();
}
