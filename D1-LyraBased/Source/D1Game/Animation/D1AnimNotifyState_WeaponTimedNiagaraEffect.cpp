#include "D1AnimNotifyState_WeaponTimedNiagaraEffect.h"

#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_WeaponTimedNiagaraEffect)

UD1AnimNotifyState_WeaponTimedNiagaraEffect::UD1AnimNotifyState_WeaponTimedNiagaraEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	
	Template = nullptr;
	LocationOffset.Set(0.0f, 0.0f, 0.0f);
	RotationOffset = FRotator(0.0f, 0.0f, 0.0f);
}

void UD1AnimNotifyState_WeaponTimedNiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::NotifyBegin(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, TotalDuration, EventReference);
}

void UD1AnimNotifyState_WeaponTimedNiagaraEffect::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::NotifyEnd(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, EventReference);
}

USkeletalMeshComponent* UD1AnimNotifyState_WeaponTimedNiagaraEffect::GetWeaponMeshComponent(USkeletalMeshComponent* CharacterMeshComponent) const
{
	USkeletalMeshComponent* WeaponMeshComponent = nullptr;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(CharacterMeshComponent->GetOwner()))
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
