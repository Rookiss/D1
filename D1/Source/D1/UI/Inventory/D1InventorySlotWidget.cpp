#include "D1InventorySlotWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotWidget)

UD1InventorySlotWidget::UD1InventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SizeBox_Root->SetWidthOverride(SlotSize.X);
	SizeBox_Root->SetHeightOverride(SlotSize.Y);
}

void UD1InventorySlotWidget::ChangeSlotColor(ESlotColor SlotColor)
{
	Image_Blue->SetRenderOpacity(0.f);
	
	switch (SlotColor)
	{
	case ESlotColor::Normal:
		break;
	case ESlotColor::Blue:
		Image_Blue->SetRenderOpacity(1.f);
		break;
	case ESlotColor::Green:
		break;
	case ESlotColor::Red:
		break;
	}
}
