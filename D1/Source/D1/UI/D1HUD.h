#pragma once

#include "GameFramework/HUD.h"
#include "D1HUD.generated.h"

class UD1ItemInstance;
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

	void ShowInteractionPress(const FText& InteractionTitle, const FText& InteractionContent);
	void ShowInteractionProgress(float HoldTime);
	void HideInteractionWidget();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1SceneWidget> SceneWidget;
};
