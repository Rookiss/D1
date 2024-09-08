#pragma once

#include "Blueprint/UserWidget.h"
#include "D1SkillIconWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class UD1SkillIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1SkillIconWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
    UFUNCTION()
    void OnSkillIconClicked();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Cooldown;
};
