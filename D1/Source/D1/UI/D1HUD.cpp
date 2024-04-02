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

void AD1HUD::ShowControlledInventoryWidget()
{
	if (SceneWidget)
	{
		SceneWidget->ShowControlledInventoryWidget();
	}
}

void AD1HUD::HideControlledInventoryWidget()
{
	if (SceneWidget)
	{
		SceneWidget->HideControlledInventoryWidget();
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

void AD1HUD::ShowSpellSelectWidget()
{
	if (SceneWidget)
	{
		SceneWidget->ShowSpellSelectWidget();
	}
}

void AD1HUD::HideSpellSelectWidget()
{
	if (SceneWidget)
	{
		SceneWidget->HideSpellSelectWidget();
	}
}

void AD1HUD::ShowSpellProgressWidget(const FText& SpellName, float CastTime)
{
	if (SceneWidget)
	{
		SceneWidget->ShowSpellProgressWidget(SpellName, CastTime);
	}
}

void AD1HUD::HideSpellProgressWidget()
{
	if (SceneWidget)
	{
		SceneWidget->HideSpellProgressWidget();
	}
}

UD1PlayerInventoryWidget* AD1HUD::GetControlledInventoryWidget() const
{
	return SceneWidget ? SceneWidget->GetControlledInventoryWidget() : nullptr;
}

UD1InteractionWidget* AD1HUD::GetInteractionWidget() const
{
	return SceneWidget ? SceneWidget->GetInteractionWidget() : nullptr;
}

UD1SpellSelectWidget* AD1HUD::GetSpellSelectWidget() const
{
	return SceneWidget ? SceneWidget->GetSpellSelectWidget() : nullptr;
}

UD1SpellProgressWidget* AD1HUD::GetSpellProgressWidget() const
{
	return SceneWidget ? SceneWidget->GetSpellProgressWidget() : nullptr;
}
