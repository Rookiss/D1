#pragma once

#include "Blueprint/UserWidget.h"
#include "D1ElectricFieldCircleWidget.generated.h"

class UImage;
class UCanvasPanelSlot;

UCLASS()
class UD1ElectricFieldCircleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ElectricFieldCircleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ElectricFieldCircle;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> CircleDynamicMaterial;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> CanvasPanelSlot;

private:
	UPROPERTY(EditAnywhere)
	float Thickness = 500.f;
};
