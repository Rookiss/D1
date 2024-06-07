#include "D1ActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ActivatableWidget)

UD1ActivatableWidget::UD1ActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

FReply UD1ActivatableWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	if (DeactivateKey.IsValid() && InKeyEvent.GetKey() == DeactivateKey && InKeyEvent.IsRepeat() == false)
	{
		DeactivateWidget();
		return FReply::Handled();
	}
	
	return Reply;
}
