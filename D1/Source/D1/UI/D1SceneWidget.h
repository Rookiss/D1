#pragma once

#include "D1UserWidget.h"
#include "D1SceneWidget.generated.h"

class UD1ItemInstance;
class UD1PlayerInventoryWidget;
class UD1ItemHoverWidget;
class UD1InteractionWidget;

UCLASS()
class UD1SceneWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1SceneWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	void ShowControlledPlayerInventoryWidget();
	void HideControlledPlayerInventoryWidget();
	
	void ShowItemHoverWidget(UD1ItemInstance* ItemInstance);
	void HideItemHoverWidget();
	
	void ShowInteractionPressWidget(const FText& InteractionTitle, const FText& InteractionContent);
	void ShowInteractionHasDurationWidget(float HoldTime);
	void HideInteractionWidget();

public:
	bool IsAllMouseInteractionWidgetHidden() const;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1PlayerInventoryWidget> ControlledPlayerInventoryWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ItemHoverWidget> ItemHoverWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1InteractionWidget> InteractionWidget;
};
