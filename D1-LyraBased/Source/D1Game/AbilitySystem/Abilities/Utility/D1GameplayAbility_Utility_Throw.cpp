#include "D1GameplayAbility_Utility_Throw.h"

#include "Actors/D1PickupableItemBase.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ItemData.h"
#include "Interaction/D1Pickupable.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Utility.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_Throw)

UD1GameplayAbility_Utility_Throw::UD1GameplayAbility_Utility_Throw(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

FTransform UD1GameplayAbility_Utility_Throw::GetSpawnTransform()
{
	if (WeaponActor == nullptr)
		return FTransform::Identity;
	
	FTransform SpawnTransform = FTransform::Identity;
	
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		int32 ItemTemplateID = WeaponActor->GetTemplateID();
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

		if (const UD1ItemFragment_Equippable_Utility* UtilityFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Utility>())
		{
			SpawnTransform = UtilityFragment->WeaponAttachInfo.AttachTransform;
			const FTransform& SocketTransform = LyraCharacter->GetMesh()->GetSocketTransform(UtilityFragment->WeaponAttachInfo.AttachSocket);
			SpawnTransform *= SocketTransform;
			
			FRotator LocalRotation = FRotator(0.f, 180.f, 90.f);
			SpawnTransform.SetRotation(UKismetMathLibrary::TransformRotation(LyraCharacter->GetTransform(), LocalRotation).Quaternion()); // Only for Torch
		}
	}

	return SpawnTransform;
}

void UD1GameplayAbility_Utility_Throw::SpawnThrowableItem()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetLyraCharacterFromActorInfo();
	
	AD1PickupableItemBase* PickupableItemActor = GetWorld()->SpawnActor<AD1PickupableItemBase>(ThrowableItemClass, GetSpawnTransform(), SpawnParameters);
	
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetLyraCharacterFromActorInfo());
	if (LyraCharacter == nullptr)
		return;

	UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return;

	EEquipmentSlotType EquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(WeaponHandType, EquipManager->GetCurrentEquipState());
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	if (EquipmentManager == nullptr)
		return;
	
	FD1PickupInfo PickupInfo;
	PickupInfo.PickupInstance.ItemInstance = EquipmentManager->GetItemInstance(EquipmentSlotType);
	PickupInfo.PickupInstance.ItemCount = 1;
	PickupableItemActor->SetPickupInfo(PickupInfo);
}
