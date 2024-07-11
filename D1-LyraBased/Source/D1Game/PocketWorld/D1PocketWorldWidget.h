#pragma once

#include "Blueprint/UserWidget.h"
#include "D1PocketWorldWidget.generated.h"

class UImage;

UCLASS()
class UD1PocketWorldWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1PocketWorldWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void RefreshUI();

private:
	UFUNCTION()
	void OnPocketStageReady(AD1PocketStage* PocketStage);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Render;

private:
	UPROPERTY()
	TWeakObjectPtr<AD1PocketStage> CachedPocketStage;
	
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> DiffuseRenderTarget;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> AlphaRenderTarget;

private:
	FTimerHandle TickTimerHandle;
	FDelegateHandle DelegateHandle;
};
