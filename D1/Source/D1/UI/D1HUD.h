#pragma once

#include "D1SceneWidget.h"
#include "GameFramework/HUD.h"
#include "D1HUD.generated.h"

class UD1InteractionWidget;
class UD1SceneWidget;

UCLASS()
class AD1HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AD1HUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void ShowSceneWidget();

	void ShowControlledPlayerInventoryWidget();
	void HideControlledPlayerInventoryWidget();
	
	void ShowItemHoverWidget(UD1ItemInstance* ItemInstance);
	void HideItemHoverWidget();
	
public:
	UD1SceneWidget* GetSceneWidget() const { return SceneWidget; }
	UD1InteractionWidget* GetInteractionWidget() const { return SceneWidget ? SceneWidget->GetInteractionWidget() : nullptr; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1SceneWidget> SceneWidget;
};
