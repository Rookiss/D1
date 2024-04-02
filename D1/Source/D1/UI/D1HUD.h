#pragma once

#include "GameFramework/HUD.h"
#include "D1HUD.generated.h"

class UD1SpellProgressWidget;
class UD1SpellSelectWidget;
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

	UFUNCTION(BlueprintCallable)
	void ShowItemHoverWidget(UD1ItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	void HideItemHoverWidget();

	UFUNCTION(BlueprintCallable)
	void ShowSpellSelectWidget();

	UFUNCTION(BlueprintCallable)
	void HideSpellSelectWidget();

	UFUNCTION(BlueprintCallable)
	void ShowSpellProgressWidget(const FText& SpellName, float CastTime);

	UFUNCTION(BlueprintCallable)
	void HideSpellProgressWidget();

public:
	UD1SceneWidget* GetSceneWidget() const { return SceneWidget; }

	UFUNCTION(BlueprintCallable)
	UD1PlayerInventoryWidget* GetControlledInventoryWidget() const;

	UFUNCTION(BlueprintCallable)
	UD1InteractionWidget* GetInteractionWidget() const;

	UFUNCTION(BlueprintCallable)
	UD1SpellSelectWidget* GetSpellSelectWidget() const;

	UFUNCTION(BlueprintCallable)
	UD1SpellProgressWidget* GetSpellProgressWidget() const;

protected:
	UPROPERTY()
	TObjectPtr<UD1SceneWidget> SceneWidget;
};
