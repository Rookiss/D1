#include "D1InventoryEntryWidget.h"

#include "Components/SizeBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryEntryWidget)

UD1InventoryEntryWidget::UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryEntryWidget::SetEntrySize(const FIntPoint& Size)
{
	SizeBox_Entry->SetWidthOverride(Size.X * 50);
	SizeBox_Entry->SetHeightOverride(Size.Y * 50);
}
