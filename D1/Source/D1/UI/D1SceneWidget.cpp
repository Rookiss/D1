#include "D1SceneWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SceneWidget)

UD1SceneWidget::UD1SceneWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SceneWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		
		PC->FlushPressedKeys();
		PC->SetShowMouseCursor(true);
	}
}
