#include "D1InventoryDragWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Inventory/Fragments/D1ItemFragment_Stackable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryDragWidget)

UD1InventoryDragWidget::UD1InventoryDragWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryDragWidget::Init(const FVector2D& WidgetSize, UTexture2D* ItemIcon, int32 ItemCount)
{
	SizeBox_Root->SetWidthOverride(WidgetSize.X);
	SizeBox_Root->SetHeightOverride(WidgetSize.Y);
	
	Image_Icon->SetBrushFromTexture(ItemIcon);
	Text_Count->SetText((ItemCount >= 2) ? FText::AsNumber(ItemCount) : FText::GetEmpty());
}
