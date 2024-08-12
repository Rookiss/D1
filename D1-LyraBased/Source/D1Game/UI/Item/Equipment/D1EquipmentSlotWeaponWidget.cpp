#include "D1EquipmentSlotWeaponWidget.h"

#include "D1Define.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "D1EquipmentEntryWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotWeaponWidget)

UD1EquipmentSlotWeaponWidget::UD1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1EquipmentSlotWeaponWidget::Init(EWeaponSlotType InWeaponSlotType, UD1EquipmentManagerComponent* InEquipmentManager)
{
	check(InWeaponSlotType != EWeaponSlotType::Count && InEquipmentManager != nullptr);
	
	WeaponSlotType = InWeaponSlotType;
	EquipmentManager = InEquipmentManager;
}

void UD1EquipmentSlotWeaponWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image_Icon_Left->SetBrushFromTexture(WeaponIconTexture_Left, true);
	Image_Icon_Right->SetBrushFromTexture(WeaponIconTexture_Right, true);
}

void UD1EquipmentSlotWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EntryWidgets.SetNum((int32)EWeaponHandType::Count);
	SlotImages   = { Image_Slot_LeftHand, Image_Slot_RightHand, Image_Slot_TwoHand };
	SlotOverlays = { Overlay_Slot_LeftHand, Overlay_Slot_RightHand, Overlay_Slot_TwoHand };
}

bool UD1EquipmentSlotWeaponWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;
	
	bAlreadyHovered = true;
	
	UD1ItemDragDrop* ItemDragDrop = Cast<UD1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;
	
	UD1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;
	
	bool bIsValid = false;
	UImage* TargetImage = Image_Slot_TwoHand;
	
	if (const UD1ItemFragment_Equippable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		TargetImage = SlotImages[(int32)FromWeaponFragment->WeaponHandType];
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (UD1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
			{
				FIntPoint OutToItemSlotPos;
				bIsValid = EquipmentManager->CanSwapEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType, OutToItemSlotPos);
			}
			else
			{
				bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType) > 0;
			}
		}
		else if (UD1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
			{
				bIsValid = EquipmentManager->CanSwapEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
			}
			else
			{
				bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType) > 0;
			}
		}
	}
	
	ChangeHoverState(TargetImage, bIsValid ? ESlotState::Valid : ESlotState::Invalid);
	return true;
}

bool UD1EquipmentSlotWeaponWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FinishDrag();
	
	UD1ItemDragDrop* ItemDragDrop = Cast<UD1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;
	
	if (UD1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
	{
		FromEntryWidget->RefreshWidgetOpacity(true);
	}
	
	UD1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
	if (ItemManager == nullptr)
		return false;
	
	if (const UD1ItemFragment_Equippable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (UD1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
		}
	}
	return true;
}

void UD1EquipmentSlotWeaponWidget::FinishDrag()
{
	Super::FinishDrag();
	
	for (UImage* TargetImage : SlotImages)
	{
		ChangeHoverState(TargetImage, ESlotState::Default);
	}
}

void UD1EquipmentSlotWeaponWidget::OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (InWeaponHandType == EWeaponHandType::Count)
		return;

	int32 WeaponHandIndex = (int32)InWeaponHandType;
	UOverlay* TargetOverlay = SlotOverlays[WeaponHandIndex];

	if (UD1EquipmentEntryWidget* EntryWidget = EntryWidgets[WeaponHandIndex])
	{
		if (UD1ItemInstance* ItemInstance = EntryWidget->GetItemInstance())
		{
			if (ItemInstance == InItemInstance)
			{
				EntryWidget->RefreshItemCount(InItemCount);
				return;
			}
		}
		
		TargetOverlay->RemoveChild(EntryWidget);
		EntryWidgets[WeaponHandIndex] = nullptr;
	}
	
	if (InItemInstance)
	{
		UD1EquipmentEntryWidget* EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[WeaponHandIndex] = EntryWidget;
		
		UOverlaySlot* OverlaySlot = TargetOverlay->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, UD1EquipManagerComponent::ConvertToEquipmentSlotType(InWeaponHandType, WeaponSlotType), EquipmentManager);
	}

	FSlateBrush LeftBrush, RightBrush;
	
	switch (InWeaponHandType)
	{
	case EWeaponHandType::LeftHand:
		if (InItemInstance)
		{
			Image_Icon_Left->SetRenderOpacity(0.f);
		}
		else
		{
			Image_Icon_Left->SetRenderOpacity(1.f);
		}
		break;
	case EWeaponHandType::RightHand:
		if (InItemInstance)
		{
			Image_Icon_Right->SetRenderOpacity(0.f);
		}
		else
		{
			Image_Icon_Right->SetRenderOpacity(1.f);
		}
		break;
	case EWeaponHandType::TwoHand:
		if (InItemInstance)
		{
			Image_Icon_Left->SetRenderOpacity(0.f);
			Image_Icon_Right->SetRenderOpacity(0.f);

			LeftBrush = Image_Frame_Left->GetBrush();
			LeftBrush.Margin.Right = 0.f;
			Image_Frame_Left->SetBrush(LeftBrush);

			RightBrush = Image_Frame_Right->GetBrush();
			RightBrush.Margin.Left = 0.f;
			Image_Frame_Right->SetBrush(RightBrush);
		}
		else
		{
			Image_Icon_Left->SetRenderOpacity(1.f);
			Image_Icon_Right->SetRenderOpacity(1.f);

			LeftBrush = Image_Frame_Left->GetBrush();
			LeftBrush.Margin.Right = LeftBrush.Margin.Left / 2.f;
			Image_Frame_Left->SetBrush(LeftBrush);

			RightBrush = Image_Frame_Right->GetBrush();
			RightBrush.Margin.Left = LeftBrush.Margin.Right / 2.f;
			Image_Frame_Right->SetBrush(RightBrush);
		}
		break;
	}
}
