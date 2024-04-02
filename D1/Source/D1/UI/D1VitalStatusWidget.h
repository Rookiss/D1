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

private:
	void HandleHealthChanged(float NewValue);
	void HandleMaxHealthChanged(float NewValue);
	void HandleManaChanged(float NewValue);
	void HandleMaxManaChanged(float NewValue);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Mana;

private:
	float CachedHealth = 0.f;
	float CachedMaxHealth = 0.f;
	float CachedMana = 0.f;
	float CachedMaxMana = 0.f;
};
