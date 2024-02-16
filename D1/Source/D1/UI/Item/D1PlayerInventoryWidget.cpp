#include "D1PlayerInventoryWidget.h"

#include "D1Define.h"
#include "Components/TextBlock.h"
#include "Equipment/D1EquipmentSlotsWidget.h"
#include "Inventory/D1InventorySlotsWidget.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerInventoryWidget)

UD1PlayerInventoryWidget::UD1PlayerInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1PlayerInventoryWidget::Init()
{
	check(EquipmentManagerComponent);
	EquipmentSlotsWidget->Init();

	check(InventoryManagerComponent);
	InventorySlotsWidget->Init();

	CachedGoldCount = InventoryManagerComponent->GetTotalCountByID(9999); 
	Text_Gold->SetText(FText::AsNumber(CachedGoldCount));
	InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UD1PlayerInventoryWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemID, int32 OldItemCount, int32 NewItemCount)
{
	if (ItemID == ITEM_ID_COIN)
	{
		CachedGoldCount += (NewItemCount - OldItemCount);
	}
}
