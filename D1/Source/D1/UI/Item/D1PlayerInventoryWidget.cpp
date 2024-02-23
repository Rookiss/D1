#include "D1PlayerInventoryWidget.h"

#include "D1Define.h"
#include "Components/TextBlock.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Player/D1PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PlayerInventoryWidget)

UD1PlayerInventoryWidget::UD1PlayerInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1PlayerInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AD1PlayerController* PC = Cast<AD1PlayerController>(GetOwningPlayer());
	check(PC);

	InventoryManagerComponent = PC->InventoryManagerComponent;
	check(InventoryManagerComponent);

	InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
	
	Text_Gold->SetText(FText::AsNumber(InventoryManagerComponent->GetTotalCountByID(ITEM_ID_COIN)));
}

void UD1PlayerInventoryWidget::OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 NewItemCount)
{
	if (ItemInstance && ItemInstance->GetItemID() != ITEM_ID_COIN)
		return;
	
	Text_Gold->SetText(FText::AsNumber(InventoryManagerComponent->GetTotalCountByID(ITEM_ID_COIN)));
}
