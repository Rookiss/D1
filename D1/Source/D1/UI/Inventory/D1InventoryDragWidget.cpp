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

void UD1InventoryDragWidget::Init(const FVector2D& InWidgetSize, UTexture2D* InItemIcon, int32 InItemCount)
{
	SizeBox_Root->SetWidthOverride(InWidgetSize.X);
	SizeBox_Root->SetHeightOverride(InWidgetSize.Y);
	
	Image_Icon->SetBrushFromTexture(InItemIcon);
	Text_Count->SetText((InItemCount >= 2) ? FText::AsNumber(InItemCount) : FText::GetEmpty());
}
