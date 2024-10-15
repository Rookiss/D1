#include "D1GameplayAbility_Skill_AOE.h"

#include "Actors/D1AOEBase.h"
#include "Character/LyraCharacter.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Skill_AOE)

UD1GameplayAbility_Skill_AOE::UD1GameplayAbility_Skill_AOE(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Skill_AOE::SpawnAOE()
{
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
