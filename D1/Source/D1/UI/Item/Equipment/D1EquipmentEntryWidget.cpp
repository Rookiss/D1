#include "D1EquipmentEntryWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Character/D1Player.h"
#include "Components/SizeBox.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Player/D1PlayerController.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Drag/D1ItemDragWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentEntryWidget)

UD1EquipmentEntryWidget::UD1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentEntryWidget::Init(UD1ItemInstance* InItemInstance, EEquipmentSlotType InEquipmentSlotType)
{
	if (InEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || InEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || InEquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	SetItemInstance(InItemInstance);
	EquipmentSlotType = InEquipmentSlotType;
}

void UD1EquipmentEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AD1Player* PlayerPawn = Cast<AD1Player>(GetOwningPlayerPawn());
	check(PlayerPawn);
	
	EquipmentManagerComponent = PlayerPawn->EquipmentManagerComponent;
	check(EquipmentManagerComponent);
}

void UD1EquipmentEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemTemplate& ItemDef = ItemData->FindItemTemplateByID(ItemInstance->GetTemplateID());
	
	UD1ItemDragWidget* DragWidget = CreateWidget<UD1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D EntityWidgetSize = FVector2D(ItemDef.SlotCount * Item::UnitInventorySlotSize);
	DragWidget->Init(EntityWidgetSize, ItemDef.IconTexture, 1);
	
	UD1ItemDragDrop* DragDrop = NewObject<UD1ItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::CenterCenter;
	DragDrop->FromEntryWidget = this;
	DragDrop->FromEquipmentManager = EquipmentManagerComponent;
	DragDrop->FromEquipmentSlotType = EquipmentSlotType;
	DragDrop->ItemInstance = ItemInstance;
	DragDrop->DeltaWidgetPos = (EntityWidgetSize / 2.f) - (Item::UnitInventorySlotSize / 2.f);
	OutOperation = DragDrop;
}
