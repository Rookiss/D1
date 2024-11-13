#include "D1MiniMapWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MiniMapWidget)

UD1MiniMapWidget::UD1MiniMapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1MiniMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	HidePinIcon();
}

void UD1MiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetSlot = Cast<UCanvasPanelSlot>(Slot);
	PinIconSlot = Cast<UCanvasPanelSlot>(Image_PinIcon->Slot);
	PlayerIconSlot = Cast<UCanvasPanelSlot>(Image_PlayerIcon->Slot);
	MiniMapPanelSlot = Cast<UCanvasPanelSlot>(CanvasPanel_MiniMap->Slot);
	InitialMiniMapSize = MiniMapPanelSlot->GetSize();
}

void UD1MiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	APawn* Pawn = GetOwningPlayerPawn();
	if (IsValid(Pawn))
	{
		FVector PawnLocation = Pawn->GetActorLocation();
		MiniMapPanelSlot->SetPosition(PawnWorldPosToMiniMapPanelPos(FVector2D(PawnLocation.X, PawnLocation.Y)));
	}
	
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			CanvasPanel_Player->SetRenderTransformAngle(CameraManager->GetCameraRotation().Yaw);
		}
	}
}

void UD1MiniMapWidget::ShowPinIcon(const FVector& PinWorldPos)
{
	Image_PinIcon->SetVisibility(ESlateVisibility::Visible);
	InitialIconPos = PinWorldPosToMiniMapInitialPos(FVector2D(PinWorldPos.X, PinWorldPos.Y));
}

void UD1MiniMapWidget::HidePinIcon()
{
	Image_PinIcon->SetVisibility(ESlateVisibility::Hidden);
}

FVector2D UD1MiniMapWidget::PawnWorldPosToMiniMapPanelPos(const FVector2D& PawnWorldPos)
{
	float x = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.Y, WorldSecondPos.Y),
		FVector2D(WidgetFirstPos.X, WidgetSecondPos.X),
		PawnWorldPos.Y);

	float y = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.X, WorldSecondPos.X),
		FVector2D(WidgetFirstPos.Y, WidgetSecondPos.Y),
		PawnWorldPos.X);
	
	return FVector2D(x, y);
}

FVector2D UD1MiniMapWidget::PinWorldPosToMiniMapInitialPos(const FVector2D& PinWorldPos)
{
	float x = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.Y, WorldSecondPos.Y),
		FVector2D(-WidgetFirstPos.X, -WidgetSecondPos.X),
		PinWorldPos.Y);

	float y = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.X, WorldSecondPos.X),
		FVector2D(-WidgetFirstPos.Y, -WidgetSecondPos.Y),
		PinWorldPos.X);
	
	return FVector2D(x, y);
}
