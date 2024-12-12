#pragma once

#include "Blueprint/UserWidget.h"
#include "Character/LyraHealthComponent.h"
#include "D1DamageScreenWidget.generated.h"

class UImage;

UCLASS()
class UD1DamageScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1DamageScreenWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnHealthChanged(ULyraHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	void PlayForward();
	void PlayReverse();
	void SetTimerPlayReverse();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Blood;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Animation_FadeIn;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Damage Screen")
	TArray<TObjectPtr<UMaterialInterface>> ScreenMaterials;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage Screen")
	float WaitDuration = 2.f;

private:
	UPROPERTY()
	TObjectPtr<UUMGSequencePlayer> SequencePlayer;
	
	FTimerHandle ReverseTimerHandle;
};
