#pragma once

#include "GameFramework/HUD.h"
#include "D1HUD.generated.h"

class UD1PlayerInventoryWidget;
class UD1ItemInstance;
class UD1SceneWidget;
class UD1InteractionWidget;

UCLASS()
class AD1HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AD1HUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void ShowSceneWidget();

	UFUNCTION(BlueprintCallable)
	void ShowControlledInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void HideControlledInventoryWidget();
	
	void ShowItemHoverWidget(UD1ItemInstance* ItemInstance);
	void HideItemHoverWidget();
	
public:
	UD1SceneWidget* GetSceneWidget() const { return SceneWidget; }

	UFUNCTION(BlueprintCallable)
	UD1PlayerInventoryWidget* GetControlledInventoryWidget() const;

	UFUNCTION(BlueprintCallable)
	UD1InteractionWidget* GetInteractionWidget() const;
	
protected:
	UPROPERTY()
	TObjectPtr<UD1SceneWidget> SceneWidget;
};
