#pragma once

#include "D1UserWidget.h"
#include "D1SpellProgressWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class UD1SpellProgressWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1SpellProgressWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void ShowWidget(const FText& SpellName, float CastTime);

	UFUNCTION(BlueprintCallable)
	void NotifyWidget();
	
	UFUNCTION(BlueprintCallable)
	void HideWidget();
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_SpellProgress;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SpellName;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor DefaultColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor NotifyColor;
	
private:
	float PassedCastTime = 0.f;
	float ProgressTickTime = 0.025f;
	FTimerHandle ProgressTimerHandle;
};
