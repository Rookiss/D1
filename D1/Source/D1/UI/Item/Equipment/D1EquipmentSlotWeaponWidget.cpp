#include "D1EquipmentSlotWeaponWidget.h"

#include "D1Define.h"
#include "Character/D1Player.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "D1EquipmentEntryWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

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
	SlotImages   = { Image_Slot_Left, Image_Slot_Right, Image_Slot_Two };
	SlotOverlays = { Overlay_Slot_Left, Overlay_Slot_Right, Overlay_Slot_Two };

	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidget");
}

bool UD1EquipmentSlotWeaponWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;

	bAlreadyHovered = true;

	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	UD1ItemInstance* FromItemInstance = DragDrop->ItemInstance;
	check(FromItemInstance);

	bool bIsValid = false;
	UImage* TargetImage = Image_Slot_Two;
	
	if (const UD1ItemFragment_Equippable_Weapon* FromWeapon = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		TargetImage = SlotImages[static_cast<int32>(FromWeapon->WeaponHandType)];
		EEquipmentSlotType ToEquipmentSlotType = ConvertToEquipmentSlotType(FromWeapon->WeaponHandType);
	
		if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
		{
			bIsValid = EquipmentManagerComponent->CanEquipItem_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType);	
		}
		else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
		{
			bIsValid = EquipmentManagerComponent->CanEquipItem_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
		}
	}

	ChangeHoverState(TargetImage, bIsValid ? ESlotState::Valid : ESlotState::InValid);
	return true;
}

void UD1EquipmentSlotWeaponWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	FinishDrag();
}

bool UD1EquipmentSlotWeaponWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FinishDrag();

	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	if (UD1InventoryEntryWidget* InventoryEntryWidget = DragDrop->InventoryEntryWidget)
	{
		InventoryEntryWidget->RefreshWidgetOpacity(true);
	}
	else if (UD1EquipmentEntryWidget* EquipmentEntryWidget = DragDrop->EquipmentEntryWidget)
	{
		EquipmentEntryWidget->RefreshWidgetOpacity(true);
	}

	UD1ItemInstance* FromItemInstance = DragDrop->ItemInstance;
	check(FromItemInstance);

	const UD1ItemFragment_Equippable_Weapon* FromWeapon = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();	
	if (FromWeapon == nullptr)
		return true;

	EEquipmentSlotType ToEquipmentSlotType = ConvertToEquipmentSlotType(FromWeapon->WeaponHandType);

	if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
	{
		EquipmentManagerComponent->Server_RequestEquipItem_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType);
	}
	else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
	{
		EquipmentManagerComponent->Server_RequestEquipItem_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
	}
	
	return true;
}

void UD1EquipmentSlotWeaponWidget::FinishDrag()
{
	for (UImage* TargetImage : SlotImages)
	{
		ChangeHoverState(TargetImage, ESlotState::Default);
	}

	bAlreadyHovered = false;
}

void UD1EquipmentSlotWeaponWidget::OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UD1ItemInstance* NewItemInstance)
{
	if (InWeaponHandType == EWeaponHandType::Count)
		return;

	int32 WeaponHandIndex = static_cast<int32>(InWeaponHandType);
	UOverlay* TargetOverlay = SlotOverlays[WeaponHandIndex];
	
	if (NewItemInstance)
	{
		if (UD1EquipmentEntryWidget* EntryWidget = EntryWidgets[WeaponHandIndex])
		{
			TargetOverlay->RemoveChild(EntryWidget);
			EntryWidgets[WeaponHandIndex] = nullptr;
		}

		UD1EquipmentEntryWidget* EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetWorld(), EntryWidgetClass);
		EntryWidgets[WeaponHandIndex] = EntryWidget;
		TargetOverlay->AddChildToOverlay(EntryWidget);

		UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(EntryWidget->Slot);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(FVector2D(), NewItemInstance, ConvertToEquipmentSlotType(InWeaponHandType));
	}
	else
	{
		TargetOverlay->RemoveChild(EntryWidgets[WeaponHandIndex]);
		EntryWidgets[WeaponHandIndex] = nullptr;
	}
}

EEquipmentSlotType UD1EquipmentSlotWeaponWidget::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType) const
{
	EEquipmentSlotType EquipmentSlotType = EquipmentSlotCount;

	if (WeaponSlotType == EWeaponSlotType::Primary)
	{
		if (WeaponHandType == EWeaponHandType::LeftHand)
		{
			EquipmentSlotType = Primary_LeftHand;
		}
		else if (WeaponHandType == EWeaponHandType::RightHand)
		{
			EquipmentSlotType = Primary_RightHand;
		}
		else if (WeaponHandType == EWeaponHandType::TwoHand)
		{
			EquipmentSlotType = Primary_TwoHand;
		}
	}
	else if (WeaponSlotType == EWeaponSlotType::Secondary)
	{
		if (WeaponHandType == EWeaponHandType::LeftHand)
		{
			EquipmentSlotType = Secondary_LeftHand;
		}
		else if (WeaponHandType == EWeaponHandType::RightHand)
		{
			EquipmentSlotType = Secondary_RightHand;
		}
		else if (WeaponHandType == EWeaponHandType::TwoHand)
		{
			EquipmentSlotType = Secondary_TwoHand;
		}
	}
	
	return EquipmentSlotType;
}
