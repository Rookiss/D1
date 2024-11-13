#pragma once

#include "Blueprint/UserWidget.h"
#include "D1MiniMapWidget.generated.h"

class UImage;
class UCanvasPanel;
class UCanvasPanelSlot;

UCLASS()
class UD1MiniMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1MiniMapWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void ShowPinIcon(const FVector& PinWorldPos);
	void HidePinIcon();
	
private:
	FVector2D PawnWorldPosToMiniMapPanelPos(const FVector2D& PawnWorldPos);
	FVector2D PinWorldPosToMiniMapInitialPos(const FVector2D& PinWorldPos);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> WidgetSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> PinIconSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> PlayerIconSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> MiniMapPanelSlot;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_MiniMap;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_PinIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_PlayerIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Player;

private:
	FVector2D InitialMiniMapSize;
	FVector2D InitialIconPos;

private:
	FVector2D WorldFirstPos = FVector2D(14415.f, -14415.f);
	FVector2D WorldSecondPos = FVector2D(-13993.f, 14415.f);
	
	FVector2D WidgetFirstPos = FVector2D(500.f, 500.f);
	FVector2D WidgetSecondPos = FVector2D(-500.f, -500.f);
};
