#include "D1EquipmentSlotUtilityWidget.h"

#include "Components/Image.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/D1ItemEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotUtilityWidget)

UD1EquipmentSlotUtilityWidget::UD1EquipmentSlotUtilityWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotUtilityWidget::Init(EUtilitySlotType InUtilitySlotType, UD1EquipmentManagerComponent* InEquipmentManager)
{
	if (InUtilitySlotType != EUtilitySlotType::Count)
	{
		UtilitySlotType = InUtilitySlotType;
		EquipmentManager = InEquipmentManager;
	}
}

void UD1EquipmentSlotUtilityWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image_Icon->SetBrushFromTexture(UtilityIconTexture, true);
}

bool UD1EquipmentSlotUtilityWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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
	EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(UtilitySlotType);
	
	if (UD1InventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
	{
		if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
		{
			FIntPoint OutToItemSlotPos;
			bIsValid = EquipmentManager->CanSwapEquipment(FromInventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType, OutToItemSlotPos);
		}
		else
		{
			bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType) > 0;
		}
	}
	else if (UD1EquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
	{
		if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
		{
			bIsValid = EquipmentManager->CanSwapEquipment(FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
		}
		else
		{
			bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType) > 0;
		}
	}
	
	ChangeHoverState(Image_Slot, bIsValid ? ESlotState::Valid : ESlotState::Invalid);
	return true;
}

void UD1EquipmentSlotUtilityWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	CleanUpDrag();
}

bool UD1EquipmentSlotUtilityWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	CleanUpDrag();

	// UD1ItemDragDrop* ItemDragDrop = Cast<UD1ItemDragDrop>(InOperation);
	// check(ItemDragDrop);
	//
	// UD1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget;
	// FromEntryWidget->RefreshWidgetOpacity(true);
	//
	// UD1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	// check(FromItemInstance);
	//
	// UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
	// check(ItemManager);
	//
	// if (FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>())
	// {
	// 	EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	//
	// 	if (UD1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	// 	{
	// 		ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
	// 	}
	// 	else if (UD1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	// 	{
	// 		ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
	// 	}
	// }
	// return true;
}

void UD1EquipmentSlotUtilityWidget::CleanUpDrag()
{
	ChangeHoverState(Image_Slot, ESlotState::Default);
	bAlreadyHovered = false;
}

void UD1EquipmentSlotUtilityWidget::OnEquipmentEntryChanged(UD1ItemInstance* NewItemInstance, int32 NewItemCount)
{
	// if (EntryWidget)
	// {
	// 	Overlay_Root->RemoveChild(EntryWidget);
	// 	EntryWidget = nullptr;
	// }
	//
	// if (NewItemInstance)
	// {
	// 	EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
	// 	
	// 	UOverlaySlot* OverlaySlot = Overlay_Root->AddChildToOverlay(EntryWidget);
	// 	OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
	// 	OverlaySlot->SetVerticalAlignment(VAlign_Fill);
	// 	
	// 	EntryWidget->Init(NewItemInstance, UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType), EquipmentManager);
	//
	// 	Image_Icon->SetRenderOpacity(0.f);
	// }
	// else
	// {
	// 	Image_Icon->SetRenderOpacity(1.f);
	// }
}
