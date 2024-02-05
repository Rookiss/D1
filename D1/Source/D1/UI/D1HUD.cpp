#include "D1HUD.h"

#include "D1SceneWidget.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1HUD)

AD1HUD::AD1HUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void AD1HUD::ShowSceneWidget()
{
	if (SceneWidget)
		return;

	if (TSubclassOf<UD1SceneWidget> SceneWidgetClass = UD1AssetManager::GetSubclassByName<UD1SceneWidget>("SceneWidget"))
	{
		SceneWidget = CreateWidget<UD1SceneWidget>(GetOwningPlayerController(), SceneWidgetClass);
		SceneWidget->AddToViewport();
	}
}

void AD1HUD::ShowInteractionWidget(const FText& InteractionText)
{
	// TODO
}

void AD1HUD::HideInteractionWidget()
{
	// TODO
}

void AD1HUD::ShowControlledPlayerInventoryWidget()
{
	if (SceneWidget)
	{
		SceneWidget->ShowControlledPlayerInventoryWidget();
	}
}

void AD1HUD::HideControlledPlayerInventoryWidget()
{
	if (SceneWidget)
	{
		SceneWidget->HideControlledPlayerInventoryWidget();
	}
}

void AD1HUD::ShowItemHoverWidget(UD1ItemInstance* ItemInstance)
{
	if (SceneWidget)
	{
		SceneWidget->ShowItemHoverWidget(ItemInstance);
	}
}

void AD1HUD::HideItemHoverWidget()
{
	if (SceneWidget)
	{
		SceneWidget->HideItemHoverWidget();
	}
}
