#include "D1HUD.h"

#include "D1UserWidget.h"
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

	if (TSubclassOf<UD1UserWidget> SceneWidgetClass = UD1AssetManager::GetSubclassByName<UD1UserWidget>("SceneWidget"))
	{
		SceneWidget = CreateWidget<UD1UserWidget>(GetOwningPlayerController(), SceneWidgetClass);
		SceneWidget->AddToViewport();	
	}
}
