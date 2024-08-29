#include "D1AnimNotifyState_OverlayEffect.h"

#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Components/TimelineComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_OverlayEffect)

UD1AnimNotifyState_OverlayEffect::UD1AnimNotifyState_OverlayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// TimelineComponent = CreateDefaultSubobject<UTimelineComponent>("TimelineComponent");
}

void UD1AnimNotifyState_OverlayEffect::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (OverlayTargetType == EOverlayTargetType::None)
		return;
	
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);
	
	switch (OverlayTargetType)
	{
	case EOverlayTargetType::Weapon:
		if (USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent))
		{
			// WeaponMeshComponent->SetOverlayMaterial();
		}
		
		break;
	case EOverlayTargetType::Character:
		break;
	case EOverlayTargetType::All:
		break;
	}
}

void UD1AnimNotifyState_OverlayEffect::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);
}

void UD1AnimNotifyState_OverlayEffect::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
}

USkeletalMeshComponent* UD1AnimNotifyState_OverlayEffect::GetWeaponMeshComponent(USkeletalMeshComponent* CharacterMeshComponent) const
{
	USkeletalMeshComponent* WeaponMeshComponent = nullptr;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(CharacterMeshComponent->GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			if (AD1WeaponBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
			{
				WeaponMeshComponent = WeaponActor->WeaponMeshComponent;
			}
		}
	}

	return WeaponMeshComponent;
}
