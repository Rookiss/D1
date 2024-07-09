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
	
	EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipmentManagerComponent>();
	check(EquipmentManager);
	
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
	
		if (UD1InventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
		{
			bIsValid = EquipmentManager->CanAddEquipment(FromInventoryManager, DragDrop->FromItemSlotPos, ToEquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
		{
			bIsValid = EquipmentManager->CanAddEquipment(FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
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

	UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
	check(ItemManager);
	
	if (const UD1ItemFragment_Equippable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
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

	FSlateBrush LeftBrush, RightBrush;
	
	switch (InWeaponHandType)
	{
	case EWeaponHandType::LeftHand:
		if (NewItemInstance)
		{
			Image_Icon_Left->SetRenderOpacity(0.f);
		}
		else
		{
			Image_Icon_Left->SetRenderOpacity(1.f);
		}
		break;
	case EWeaponHandType::RightHand:
		if (NewItemInstance)
		{
			Image_Icon_Right->SetRenderOpacity(0.f);
		}
		else
		{
			Image_Icon_Right->SetRenderOpacity(1.f);
		}
		break;
	case EWeaponHandType::TwoHand:
		if (NewItemInstance)
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
