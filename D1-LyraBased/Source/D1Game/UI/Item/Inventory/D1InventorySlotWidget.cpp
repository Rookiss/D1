#include "D1InventorySlotWidget.h"

#include "Components/SizeBox.h"
#include "Data/D1UIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotWidget)

UD1InventorySlotWidget::UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FIntPoint UnitInventorySlotSize = UD1UIData::Get().UnitInventorySlotSize;
	SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}
