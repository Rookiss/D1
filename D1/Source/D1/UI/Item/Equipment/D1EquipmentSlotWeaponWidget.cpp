#include "D1EquipmentSlotWeaponWidget.h"

#include "D1Define.h"
#include "Character/D1Player.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "D1EquipmentEntryWidget.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotWeaponWidget)

UD1EquipmentSlotWeaponWidget::UD1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1EquipmentSlotWeaponWidget::Init(EWeaponSlotType InWeaponSlotType)
{
	check(InWeaponSlotType != EWeaponSlotType::Count);
	WeaponSlotType = InWeaponSlotType;
}

void UD1EquipmentSlotWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AD1Player* Player = Cast<AD1Player>(GetOwningPlayerPawn());
	check(Player);

	EquipmentManagerComponent = Player->EquipmentManagerComponent;
	check(EquipmentManagerComponent);
	
	EntryWidgets.SetNum(static_cast<int32>(EWeaponHandType::Count));
	SlotImages = { Image_Slot_Left, Image_Slot_Right, Image_Slot_Two };
	SlotOverlays = { Overlay_Slot_Left, Overlay_Slot_Right, Overlay_Slot_Two };

	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidget");
}

bool UD1EquipmentSlotWeaponWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;

	bAlreadyHovered = true;

	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	EEquipmentSlotType EquipmentSlotType = EquipmentSlotCount;
	switch (WeaponSlotType)
	{
	case EWeaponSlotType::Primary:
		
		break;
	case EWeaponSlotType::Secondary:
		
		break;
	}
	
	if (UD1ItemInstance* ItemInstance = DragDrop->ItemInstance.Get())
	{
		// if (ItemInstance->FindFragmentByClass<>()
	}

	bool bIsValid = false;

	
	
	if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
	{
		bIsValid = EquipmentManagerComponent->CanEquipItem_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, EquipmentSlotType);	
	}
	else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
	{
		bIsValid = EquipmentManagerComponent->CanEquipItem_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, EquipmentSlotType);
	}
	
	return true;
}
