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

	SizeBox_Root->SetWidthOverride(UnitSlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitSlotSize.Y);
}

void UD1InventorySlotWidget::ChangeSlotState(ESlotState InSlotState)
{
	SlotState = InSlotState;

	switch (SlotState)
	{
	case ESlotState::Default: Image_Foreground->SetColorAndOpacity(D1ItemSlotColor::DefaultColor);	break;
	case ESlotState::InValid: Image_Foreground->SetColorAndOpacity(D1ItemSlotColor::RedColor);		break;
	case ESlotState::Valid:   Image_Foreground->SetColorAndOpacity(D1ItemSlotColor::BlueColor);		break;
	}
}

void UD1InventorySlotWidget::ChangeHoverState(ESlotState InHoverState)
{
	switch (InHoverState)
	{
	case ESlotState::Default: ChangeSlotState(SlotState);										    break;
	case ESlotState::InValid: Image_Foreground->SetColorAndOpacity(D1ItemSlotColor::RedColor);	    break;
	case ESlotState::Valid:   Image_Foreground->SetColorAndOpacity(D1ItemSlotColor::GreenColor);	break;
	}
}
