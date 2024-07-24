#pragma once

#include "UI/D1ActivatableWidget.h"
#include "D1WorldMapWidget.generated.h"

class AD1ElectricField;
class UD1CompassBarWidget;
class UD1MiniMapWidget;
class UImage;
class UBorder;
class UCanvasPanel;
class UCanvasPanelSlot;
class UD1ElectricFieldCircleWidget;

UCLASS()
class UD1WorldMapWidget : public UD1ActivatableWidget
{
	GENERATED_BODY()
	
public:
	UD1WorldMapWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	bool PinIconOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	
private:
	float GetCurrentWorldMapZoom();
	FVector InitialWidgetPosToWorldPos();
	FVector2D WorldPosToInitialWidgetPos(const FVector& WorldPos);
	float WorldLengthToInitialWidgetLength(float WorldLength);

public:
	void SetMiniMapWidget(UD1MiniMapWidget* Widget) { MiniMapWidget = Widget; }
	void SetCompassBarWidget(UD1CompassBarWidget* Widget) { CompassBarWidget = Widget; }

private:
	AD1ElectricField* GetElectricFieldActor();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> PinIconSlot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> PlayerPanelSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> WorldMapPanelSlot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> CurrCircleSlot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCanvasPanelSlot> TargetCircleSlot;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UD1MiniMapWidget> MiniMapWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UD1CompassBarWidget> CompassBarWidget;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_PinIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Player;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_WorldMap;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ElectricFieldCircleWidget> CurrentCircleWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1ElectricFieldCircleWidget> TargetCircleWidget;

private:
	UPROPERTY()
	TWeakObjectPtr<AD1ElectricField> CachedElectricFieldActor;
	
private:
	float MinWorldMapZoom = 1.f;
	float MaxWorldMapZoom;
	int32 StepWorldMapZoom = 5;
	float MultiplierWorldMapZoom = 1.5f;

	float InitialInterpSpeed = 25.f;
	FVector2D InitialWorldMapSize;
	FVector2D InitialPinIconPos;

	float TargetWorldMapZoom = 1.f;
	FVector2D TargetWorldMapSize;
	FVector2D TargetWorldMapPos;

	float WidgetUnitSize;

private:
	FVector2D WorldFirstPos = FVector2D(9560.f, -9560.f);
	FVector2D WorldSecondPos = FVector2D(-9560.f, 9560.f);
	
	FVector2D WidgetFirstPos = FVector2D(-450.f, -450.f);
	FVector2D WidgetSecondPos = FVector2D(450.f, 450.f);
};
