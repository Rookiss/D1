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

void UD1InventorySlotWidget::ChangeSlotState(ESlotState InSlotState)
{
	SlotState = InSlotState;

	switch (SlotState)
	{
	case ESlotState::Default: Image_Foreground->SetColorAndOpacity(DefaultColor);	break;
	case ESlotState::InValid: Image_Foreground->SetColorAndOpacity(RedColor);		break;
	case ESlotState::Valid:   Image_Foreground->SetColorAndOpacity(BlueColor);		break;
	}
}

void UD1InventorySlotWidget::ChangeHoverState(ESlotState InHoverState)
{
	switch (InHoverState)
	{
	case ESlotState::Default: ChangeSlotState(SlotState);						break;
	case ESlotState::InValid: Image_Foreground->SetColorAndOpacity(RedColor);	break;
	case ESlotState::Valid:   Image_Foreground->SetColorAndOpacity(GreenColor);	break;
	}
}
