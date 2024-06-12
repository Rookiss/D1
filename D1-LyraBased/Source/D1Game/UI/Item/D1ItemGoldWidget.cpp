#include "D1ItemGoldWidget.h"

#include "Components/TextBlock.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemGoldWidget)

UD1ItemGoldWidget::UD1ItemGoldWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemGoldWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PlayerController = GetOwningPlayer();
	check(PlayerController);

	InventoryManagerComponent = PlayerController->GetComponentByClass<UD1InventoryManagerComponent>();
	check(InventoryManagerComponent);

	RefreshGoldCount();
	InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UD1ItemGoldWidget::RefreshGoldCount()
{
	int32 GoldCount = InventoryManagerComponent->GetTotalCountByID(Item::GoldID);
	Text_TotalGold->SetText(FText::AsNumber(GoldCount));
}

void UD1ItemGoldWidget::OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 NewItemCount)
{
	RefreshGoldCount();
}
