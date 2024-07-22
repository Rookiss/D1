#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CompassBarWidget.generated.h"

class UImage;

UCLASS()
class UD1CompassBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CompassBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void ShowPinIcon(const FVector& WorldPos);
	void HidePinIcon();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> PinIconDynamicMaterial;
    
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> CompassDynamicMaterial;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_PinIcon;
    
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_CompassBar;

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsTargetSet;

	UPROPERTY(VisibleAnywhere)
	FVector TargetWorldPos;
};
