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
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

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
	FDelegateHandle DelegateHandle;
};
