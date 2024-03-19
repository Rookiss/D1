#include "D1GameplayAbility_Block.h"

#include "Weapon/D1WeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Block)

UD1GameplayAbility_Block::UD1GameplayAbility_Block(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Block::ChangeBlockState(bool bShouldBlock)
{
	if (WeaponActor)
	{
		if (UStaticMeshComponent* StaticMeshComponent = WeaponActor->WeaponMeshComponent)
		{
			StaticMeshComponent->SetGenerateOverlapEvents(bShouldBlock);
		}
	}
}
