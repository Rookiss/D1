#include "D1ItemSlotWidget.h"

#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemSlotWidget)

UD1ItemSlotWidget::UD1ItemSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemSlotWidget::ChangeSlotState(UImage* TargetImage, ESlotState NewSlotState)
{
	if (TargetImage == nullptr)
		return;

	ESlotState& SlotState = ImageToSlotState.FindOrAdd(TargetImage);
	SlotState = NewSlotState;

	switch (SlotState)
	{
	case ESlotState::Default: TargetImage->SetColorAndOpacity(ItemSlotColor::DefaultColor);	break;
	case ESlotState::Invalid: TargetImage->SetColorAndOpacity(ItemSlotColor::RedColor);		break;
	case ESlotState::Valid:   TargetImage->SetColorAndOpacity(ItemSlotColor::BlueColor);	break;
	}
}

void UD1ItemSlotWidget::ChangeHoverState(UImage* TargetImage, ESlotState NewHoverState)
{
	if (TargetImage == nullptr)
		return;
	
	switch (NewHoverState)
	{
	case ESlotState::Default: ChangeSlotState(TargetImage, ImageToSlotState.FindOrAdd(TargetImage));	break;
	case ESlotState::Invalid: TargetImage->SetColorAndOpacity(ItemSlotColor::RedColor);					break;
	case ESlotState::Valid:   TargetImage->SetColorAndOpacity(ItemSlotColor::GreenColor);				break;
	}
}
