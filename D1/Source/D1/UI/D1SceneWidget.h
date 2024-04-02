#pragma once

#include "D1UserWidget.h"
#include "D1SceneWidget.generated.h"

class UD1SpellProgressWidget;
class UD1SpellSelectWidget;
class UD1ItemInstance;
class UD1InteractionWidget;
class UD1PlayerInventoryWidget;
class UD1ItemHoverWidget;

UCLASS()
class UD1SceneWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1SceneWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	void ShowControlledInventoryWidget();
	void HideControlledInventoryWidget();
	
	void ShowItemHoverWidget(UD1ItemInstance* ItemInstance);
	void HideItemHoverWidget();

	void ShowSpellSelectWidget();
	void HideSpellSelectWidget();

	void ShowSpellProgressWidget(const FText& SpellName, float CastTime);
	void HideSpellProgressWidget();

public:
	UD1PlayerInventoryWidget* GetControlledInventoryWidget() const { return ControlledInventoryWidget; }
	UD1InteractionWidget* GetInteractionWidget() const { return InteractionWidget; }
	UD1SpellSelectWidget* GetSpellSelectWidget() const { return SpellSelectWidget; }
	UD1SpellProgressWidget* GetSpellProgressWidget() const { return SpellProgressWidget; }
	
	bool IsAllItemSlotWidgetHidden() const;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1PlayerInventoryWidget> ControlledInventoryWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ItemHoverWidget> ItemHoverWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1InteractionWidget> InteractionWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1SpellSelectWidget> SpellSelectWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1SpellProgressWidget> SpellProgressWidget;
};
