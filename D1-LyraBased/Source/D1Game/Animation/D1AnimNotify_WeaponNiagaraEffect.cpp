#include "D1AnimNotify_WeaponNiagaraEffect.h"

#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotify_WeaponNiagaraEffect)

UD1AnimNotify_WeaponNiagaraEffect::UD1AnimNotify_WeaponNiagaraEffect()
	: Super()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UD1AnimNotify_WeaponNiagaraEffect::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::Notify(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, EventReference);
}

USkeletalMeshComponent* UD1AnimNotify_WeaponNiagaraEffect::GetWeaponMeshComponent(USkeletalMeshComponent* MeshComponent) const
{
	USkeletalMeshComponent* WeaponMeshComponent = nullptr;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			if (AD1EquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
			{
				WeaponMeshComponent = WeaponActor->MeshComponent;
			}
		}
	}

	return WeaponMeshComponent;
}
