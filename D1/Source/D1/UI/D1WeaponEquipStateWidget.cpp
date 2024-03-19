#include "D1WeaponEquipStateWidget.h"

#include "Character/D1Player.h"
#include "Components/Overlay.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WeaponEquipStateWidget)

UD1WeaponEquipStateWidget::UD1WeaponEquipStateWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1WeaponEquipStateWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WeaponOverlays = { Overlay_Primary_LeftHand, Overlay_Primary_RightHand, Overlay_Primary_TwoHand, Overlay_Secondary_LeftHand, Overlay_Secondary_RightHand, Overlay_Secondary_TwoHand };
	
	if (AD1Player* Player = Cast<AD1Player>(GetOwningPlayerPawn()))
	{
		if (UD1EquipmentManagerComponent* EquipmentManager = Player->EquipmentManagerComponent)
		{
			const TArray<FD1EquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
			for (const FD1EquipmentEntry& Entry : Entries)
			{
				if (Entry.GetItemInstance() && UD1EquipmentManagerComponent::IsWeaponSlot(Entry.EquipmentSlotType))
				{
					OnEquipmentEntryChanged(Entry.EquipmentSlotType, Entry.GetItemInstance());
				}
			}
			EquipmentManager->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
		}

		if (UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent)
		{
			OnWeaponEquipStateChanged(EquipManager->GetCurrentWeaponEquipState());
			EquipManager->OnWeaponEquipStateChanged.AddUObject(this, &ThisClass::OnWeaponEquipStateChanged);
		}
	}
}

void UD1WeaponEquipStateWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType) == false)
		return;

	if (ItemInstance)
	{
		
	}
	else
	{
		WeaponOverlays[(int32)EquipmentSlotType - (int32)EEquipmentSlotType::Primary_LeftHand]->ClearChildren();
	}
}

void UD1WeaponEquipStateWidget::OnWeaponEquipStateChanged(EWeaponEquipState WeaponEquipState)
{
	switch (WeaponEquipState)
	{
	case EWeaponEquipState::Unarmed:
		break;
	case EWeaponEquipState::Primary:
		break;
	case EWeaponEquipState::Secondary:
		break;
	case EWeaponEquipState::Count:
		break;
	}
}
