#pragma once

#include "D1UserWidget.h"
#include "D1VitalStatusWidget.generated.h"

class UProgressBar;

UCLASS()
class UD1VitalStatusWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1VitalStatusWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void HandleHealthChanged(float NewValue);
	void HandleMaxHealthChanged(float NewValue);
	void HandleStaminaChanged(float NewValue);
	void HandleMaxStaminaChanged(float NewValue);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Stamina;

private:
	float CachedCurrentHealth = 0.f;
	float CachedTargetHealth = 0.f;
	float CachedMaxHealth = 0.f;
	float CachedCurrentStamina = 0.f;
	float CachedTargetStamina = 0.f;
	float CachedMaxStamina = 0.f;
};
