#include "D1EquipmentSlotWeaponWidget.h"

#include "D1Define.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "D1EquipmentEntryWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotWeaponWidget)

UD1EquipmentSlotWeaponWidget::UD1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1EquipmentSlotWeaponWidget::Init(EWeaponSlotType InWeaponSlotType)
{
	if (InWeaponSlotType == EWeaponSlotType::Count)
		return;
	
	WeaponSlotType = InWeaponSlotType;
}

void UD1EquipmentSlotWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EntryWidgets.SetNum((int32)EWeaponHandType::Count);
	SlotImages   = { Image_Slot_LeftHand, Image_Slot_RightHand, Image_Slot_TwoHand };
	SlotOverlays = { Overlay_Slot_LeftHand, Overlay_Slot_RightHand, Overlay_Slot_TwoHand };
	
	APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayer());
	check(PlayerController);
	
	EquipmentManagerComponent = PlayerController->GetComponentByClass<UD1EquipmentManagerComponent>();
	check(EquipmentManagerComponent);
	
	EntryWidgetClass = ULyraAssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidgetClass");
}

bool UD1EquipmentSlotWeaponWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;
	
	bAlreadyHovered = true;
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);
	
	UD1ItemInstance* FromItemInstance = DragDrop->FromItemInstance;
	check(FromItemInstance);
	
	bool bIsValid = false;
	UImage* TargetImage = Image_Slot_TwoHand;
	
	if (const UD1ItemFragment_Equippable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		TargetImage = SlotImages[(int32)FromWeaponFragment->WeaponHandType];
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
		{
			bIsValid = EquipmentManagerComponent->CanAddEquipment_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
		{
			bIsValid = EquipmentManagerComponent->CanAddEquipment_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
		}
	}
	
	ChangeHoverState(TargetImage, bIsValid ? ESlotState::Valid : ESlotState::Invalid);
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
	
	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	FromEntryWidget->RefreshWidgetOpacity(true);
	
	UD1ItemInstance* FromItemInstance = DragDrop->FromItemInstance;
	check(FromItemInstance);
	
	if (const UD1ItemFragment_Equippable_Weapon* FromWeapon = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeapon->WeaponHandType, WeaponSlotType);
	
		if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
		{
			EquipmentManagerComponent->Server_AddEquipment_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
		{
			EquipmentManagerComponent->Server_AddEquipment_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
		}
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

	int32 WeaponHandIndex = (int32)InWeaponHandType;
	UOverlay* TargetOverlay = SlotOverlays[WeaponHandIndex];

	if (UD1EquipmentEntryWidget* EntryWidget = EntryWidgets[WeaponHandIndex])
	{
		TargetOverlay->RemoveChild(EntryWidget);
		EntryWidgets[WeaponHandIndex] = nullptr;
	}
	
	if (NewItemInstance)
	{
		UD1EquipmentEntryWidget* EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[WeaponHandIndex] = EntryWidget;
		
		UOverlaySlot* OverlaySlot = TargetOverlay->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(NewItemInstance, UD1EquipManagerComponent::ConvertToEquipmentSlotType(InWeaponHandType, WeaponSlotType));
	}
}
