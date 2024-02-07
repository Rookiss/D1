#pragma once

#include "UI/D1UserWidget.h"
#include "D1InteractionWidget.generated.h"

class UImage;
class UWidgetSwitcher;
class UTextBlock;

UCLASS()
class UD1InteractionWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InteractionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void ShowInteractionPress(const FText& InteractionTitle, const FText& InteractionContent);
	void ShowInteractionProgress(float HoldTime);
	void HideInteractionWidget();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_InteractionTitle;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_InteractionContent;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Circular_Fill;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_HoldTime;
	
private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CircularMaterialInstance;

	FNumberFormattingOptions NumberFormattingOptions;
	float LeftHoldTime = 0.f;
	float TotalHoldTime = 0.f;
};
