#include "D1GameplayAbility_Weapon_Block.h"

#include "Weapon/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Block)

UD1GameplayAbility_Weapon_Block::UD1GameplayAbility_Weapon_Block(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Block::ChangeBlockState(bool bShouldBlock)
{
	if (WeaponActor)
	{
		if (UStaticMeshComponent* StaticMeshComponent = WeaponActor->WeaponMeshComponent)
		{
			StaticMeshComponent->SetGenerateOverlapEvents(bShouldBlock);
		}
	}
}
