#include "D1InventorySlotWidget.h"

#include "D1Define.h"
#include "Components/SizeBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotWidget)

UD1InventorySlotWidget::UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SizeBox_Root->SetWidthOverride(Item::UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(Item::UnitInventorySlotSize.Y);
}
