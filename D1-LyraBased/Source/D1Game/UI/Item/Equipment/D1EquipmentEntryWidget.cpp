#include "D1EquipmentEntryWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/D1ItemDragWidget.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentEntryWidget)

UD1EquipmentEntryWidget::UD1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentEntryWidget::Init(UD1ItemInstance* InItemInstance, EEquipmentSlotType InEquipmentSlotType, UD1EquipmentManagerComponent* InEquipmentManager)
{
	if (InEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || InEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || InEquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	SetItemInstance(InItemInstance);
	EquipmentSlotType = InEquipmentSlotType;
	EquipmentManager = InEquipmentManager;
}

void UD1EquipmentEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	
	UD1ItemDragWidget* DragWidget = CreateWidget<UD1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount * Item::UnitInventorySlotSize);
	DragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, 1);
	
	UD1ItemDragDrop* DragDrop = NewObject<UD1ItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::CenterCenter;
	DragDrop->FromEntryWidget = this;
	DragDrop->FromEquipmentManager = EquipmentManager;
	DragDrop->FromEquipmentSlotType = EquipmentSlotType;
	DragDrop->FromItemInstance = ItemInstance;
	DragDrop->DeltaWidgetPos = (DragWidgetSize / 2.f) - (Item::UnitInventorySlotSize / 2.f);
	OutOperation = DragDrop;
}

FReply UD1EquipmentEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
		UD1InventoryManagerComponent* MyInventoryManager = GetOwningPlayerPawn()->FindComponentByClass<UD1InventoryManagerComponent>();
		UD1EquipmentManagerComponent* MyEquipmentManager = GetOwningPlayerPawn()->FindComponentByClass<UD1EquipmentManagerComponent>();

		if (ItemManager && MyInventoryManager && MyEquipmentManager)
		{
			if (EquipmentManager == MyEquipmentManager)
			{
				ItemManager->Server_EquipmentToInventory_Quick(EquipmentManager, EquipmentSlotType, MyInventoryManager);
				return FReply::Handled();
			}
			else
			{
				ItemManager->Server_EquipmentToEquipment_Quick(EquipmentManager, EquipmentSlotType, MyEquipmentManager);
				return FReply::Handled();
			}
		}
	}

	return Reply;
}
