#include "D1EquipmentSlotArmorWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotArmorWidget)

UD1EquipmentSlotArmorWidget::UD1EquipmentSlotArmorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotArmorWidget::Init(EArmorType InArmorType)
{
	if (InArmorType != EArmorType::Count)
	{
		ArmorType = InArmorType;
	}
}

void UD1EquipmentSlotArmorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image_Icon->SetBrushFromTexture(ArmorIconTexture, true);
}

void UD1EquipmentSlotArmorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipmentManagerComponent>();
	check(EquipmentManager);
	
	EntryWidgetClass = ULyraAssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidgetClass");
}

bool UD1EquipmentSlotArmorWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

	EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
		
	if (UD1InventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
	{
		bIsValid = EquipmentManager->CanAddEquipment(FromInventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType);
	}
	else if (UD1EquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
	{
		bIsValid = EquipmentManager->CanAddEquipment(FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
	}
	
	ChangeHoverState(Image_Slot, bIsValid ? ESlotState::Valid : ESlotState::Invalid);
	return true;
}

void UD1EquipmentSlotArmorWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	CleanUpDrag();
}

bool UD1EquipmentSlotArmorWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	CleanUpDrag();

	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
    FromEntryWidget->RefreshWidgetOpacity(true);

	UD1ItemInstance* FromItemInstance = DragDrop->FromItemInstance;
	check(FromItemInstance);

	UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
	check(ItemManager);
	
	if (FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	
		if (UD1InventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
		{
			ItemManager->Server_InventoryToEquipment(FromInventoryManager, DragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
		{
			ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, DragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
		}
	}
	return true;
}

void UD1EquipmentSlotArmorWidget::CleanUpDrag()
{
	ChangeHoverState(Image_Slot, ESlotState::Default);
	bAlreadyHovered = false;
}

void UD1EquipmentSlotArmorWidget::OnEquipmentEntryChanged(UD1ItemInstance* NewItemInstance)
{
	if (EntryWidget)
	{
		Overlay_Root->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
	
	if (NewItemInstance)
	{
		EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		
		UOverlaySlot* OverlaySlot = Overlay_Root->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(NewItemInstance, UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType));

		Image_Icon->SetRenderOpacity(0.f);
	}
	else
	{
		Image_Icon->SetRenderOpacity(1.f);
	}
}
