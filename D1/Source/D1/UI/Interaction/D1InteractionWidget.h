#pragma once

#include "UI/D1UserWidget.h"
#include "D1InteractionWidget.generated.h"

class UImage;
class UTextBlock;
class UWidgetSwitcher;

UCLASS()
class UD1InteractionWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InteractionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

public:
	void ShowInteractionPressWidget(const FText& InteractionTitle, const FText& InteractionContent);
	void ShowInteractionDurationWidget(float HoldTime);
	void HideInteractionWidget();

public:
	bool IsShowingDurationWidget() const;
	
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
	
	float LeftHoldTime = 0.f;
	float TotalHoldTime = 0.f;
	FTimerHandle ProgressTimerHandle;
	float ProgressTickTime = 0.05f;
	FNumberFormattingOptions NumberFormattingOptions;
};
