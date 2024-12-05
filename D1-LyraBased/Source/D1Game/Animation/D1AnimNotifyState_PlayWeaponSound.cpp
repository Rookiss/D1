#include "D1AnimNotifyState_PlayWeaponSound.h"

#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Components/AudioComponent.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimNotifyState_PlayWeaponSound)

UD1AnimNotifyState_PlayWeaponSound::UD1AnimNotifyState_PlayWeaponSound(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1AnimNotifyState_PlayWeaponSound::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (WeaponHandType == EWeaponHandType::Count || WeaponSoundType == EWeaponSoundType::None)
		return;

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner());
	if (LyraCharacter == nullptr)
		return;
	
	UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return;
	
	UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(WeaponHandType);
	if (ItemInstance == nullptr)
		return;

	AD1EquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType);
	if (WeaponActor == nullptr)
		return;
	
	const UD1ItemFragment_Equipable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Weapon>();
	if (WeaponFragment == nullptr)
		return;

	USoundBase* SelectedSound = nullptr;
	
	switch (WeaponSoundType)
	{
	case EWeaponSoundType::Swing:	SelectedSound = WeaponFragment->AttackSwingSound;	break;
	case EWeaponSoundType::Custom:	SelectedSound = CustomSound;						break;
	}

	if (SelectedSound)
	{
		AudioComponent = UGameplayStatics::SpawnSoundAttached(SelectedSound, WeaponActor->MeshComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, 1.f, 1.f, 0.f);
	}
}

void UD1AnimNotifyState_PlayWeaponSound::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AudioComponent)
	{
		AudioComponent->DestroyComponent();
	}
	
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
}
