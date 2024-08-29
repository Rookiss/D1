#include "D1GameplayAbility_Weapon_AOE.h"

#include "Actors/D1AOEBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_AOE)

UD1GameplayAbility_Weapon_AOE::UD1GameplayAbility_Weapon_AOE(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_AOE::SpawnAOE()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr)
		return;
	
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;

				FTransform SpawnTransform = FTransform::Identity;
				SpawnTransform.SetLocation(HitResult.Location);
				
				AD1AOEBase* AOE = GetWorld()->SpawnActorDeferred<AD1AOEBase>(
					AOESpawnerClass,
					SpawnTransform,
					GetAvatarActorFromActorInfo(),
					Cast<APawn>(GetAvatarActorFromActorInfo()),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
	
				AOE->FinishSpawning(SpawnTransform);
			}
		}
	}

	TargetDataHandle.Clear();
}
