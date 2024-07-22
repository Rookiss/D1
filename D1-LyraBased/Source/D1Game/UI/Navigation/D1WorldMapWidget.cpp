#include "D1WorldMapWidget.h"

#include "D1CompassBarWidget.h"
#include "D1ElectricFieldCircleWidget.h"
#include "D1MiniMapWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GameModes/LyraGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldMapWidget)

UD1WorldMapWidget::UD1WorldMapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1WorldMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PinIconSlot = Cast<UCanvasPanelSlot>(Image_PinIcon->Slot);
	PlayerPanelSlot = Cast<UCanvasPanelSlot>(CanvasPanel_Player->Slot);
	WorldMapPanelSlot = Cast<UCanvasPanelSlot>(CanvasPanel_WorldMap->Slot);
	CurrCircleSlot = Cast<UCanvasPanelSlot>(CurrentCircleWidget->Slot);
	TargetCircleSlot = Cast<UCanvasPanelSlot>(TargetCircleWidget->Slot);
	
	InitialWorldMapSize = WorldMapPanelSlot->GetSize();
	TargetWorldMapSize = InitialWorldMapSize;

	Image_PinIcon->SetVisibility(ESlateVisibility::Hidden);
	Image_PinIcon->OnMouseButtonDownEvent.BindUFunction(this, TEXT("PinIconOnMouseButtonDown"));

	SizeConstant = (WorldPosToInitialWidgetPos(FVector(100.f, 0.f, 0.f)) - WorldPosToInitialWidgetPos(FVector(0.f, 0.f, 0.f))).Length();
}

void UD1WorldMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Refresh World Map Panel Size & Position
	FVector2D NewSize = FMath::Vector2DInterpTo(WorldMapPanelSlot->GetSize(), TargetWorldMapSize, InDeltaTime, InitialInterpSpeed);
	WorldMapPanelSlot->SetSize(NewSize);

	FVector2D NewPos = FMath::Vector2DInterpTo(WorldMapPanelSlot->GetPosition(), TargetWorldMapPos, InDeltaTime, InitialInterpSpeed);
	WorldMapPanelSlot->SetPosition(NewPos);
	
	if (FMath::IsNearlyEqual(TargetWorldMapZoom, MinWorldMapZoom))
	{
		// Reset World Map Panel Target Position
		TargetWorldMapPos = FVector2D::ZeroVector;
	}
	else
	{
		// Clamp World Map Panel Position
		FVector2D DeltaSize = WorldMapPanelSlot->GetSize() / 2.f;
		FVector2D WorldMapPos = WorldMapPanelSlot->GetPosition();
		float x = FMath::Clamp(WorldMapPos.X, -DeltaSize.X, DeltaSize.X);
		float y = FMath::Clamp(WorldMapPos.Y, -DeltaSize.Y, DeltaSize.Y);
		WorldMapPanelSlot->SetPosition(FVector2D(x, y));
	}

	// Refresh Player Panel Position/Rotation & Pin Icon Position
	APawn* Pawn = GetOwningPlayerPawn();
	if (IsValid(Pawn))
	{
		FVector2D InitialPos = WorldPosToInitialWidgetPos(Pawn->GetActorLocation());
		PlayerPanelSlot->SetPosition(InitialPos * GetCurrentWorldMapZoom());
	}

	if (ALyraPlayerController* ControllerOwner = Cast<ALyraPlayerController>(GetOwningPlayer()))
	{
		if (APlayerCameraManager* CameraManager = ControllerOwner->PlayerCameraManager)
			CanvasPanel_Player->SetRenderTransformAngle(CameraManager->GetCameraRotation().Yaw);
	}

	PinIconSlot->SetPosition(InitialPinIconPos * GetCurrentWorldMapZoom());

	// Refresh Circle Widget
	if (ALyraGameState* GameState = Cast<ALyraGameState>(UGameplayStatics::GetGameState(this)))
	{
		// if (GameState->bVisibleCurrCircle)
		// 	CurrentCircleWidget->SetVisibility(ESlateVisibility::Visible);
		// else
		// 	CurrentCircleWidget->SetVisibility(ESlateVisibility::Hidden);
		//
		// if (GameState->bVisibleTargetCircle)
		// 	TargetCircleWidget->SetVisibility(ESlateVisibility::Visible);
		// else
		// 	TargetCircleWidget->SetVisibility(ESlateVisibility::Hidden);
		
		// FVector2D InitialPos = WorldPosToInitialWidgetPos(GameState->CurrCirclePos);
		// CurrCircleSlot->SetPosition(InitialPos * GetCurrentWorldMapZoom());
		//
		// FVector2D InitialSize = FVector2D(WorldLengthToInitialWidgetLength(GameState->CurrCircleRadius * 2));
		// CurrCircleSlot->SetSize(InitialSize * GetCurrentWorldMapZoom());
		//
		// InitialPos = WorldPosToInitialWidgetPos(GameState->TargetCirclePos);
		// TargetCircleSlot->SetPosition(InitialPos * GetCurrentWorldMapZoom());
		//
		// InitialSize = InitialSize = FVector2D(WorldLengthToInitialWidgetLength(GameState->TargetCircleRadius * 2));
		// TargetCircleSlot->SetSize(InitialSize * GetCurrentWorldMapZoom());
	}
	
	FVector2D InitialSize = FVector2D(WorldLengthToInitialWidgetLength(50000 * 2));
	CurrCircleSlot->SetSize(InitialSize * GetCurrentWorldMapZoom());
	
	InitialSize = InitialSize = FVector2D(WorldLengthToInitialWidgetLength(100000 * 2));
	TargetCircleSlot->SetSize(InitialSize * GetCurrentWorldMapZoom());
}

FReply UD1WorldMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && TargetWorldMapZoom > MinWorldMapZoom)
	{
		TargetWorldMapPos = WorldMapPanelSlot->GetPosition() + InMouseEvent.GetCursorDelta();
		WorldMapPanelSlot->SetPosition(TargetWorldMapPos);
		return FReply::Handled();
	}
	
	return Reply;
}

FReply UD1WorldMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseWheel(InGeometry, InMouseEvent);

	if (FMath::Abs(TargetWorldMapZoom - GetCurrentWorldMapZoom()) > 0.25f)
		return Reply;
	
	if (InMouseEvent.GetWheelDelta() > 0.f)
	{
		if (TargetWorldMapZoom < MaxWorldMapZoom)
		{
			FVector2D WorldMapPanelHalfSize = WorldMapPanelSlot->GetSize() / 2.f;
			FVector2D WorldMapPanelPos = WorldMapPanelSlot->GetPosition();
			FVector2D MouseLocalPos = CanvasPanel_WorldMap->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			TargetWorldMapPos = -WorldMapPanelHalfSize + WorldMapPanelPos - MouseLocalPos + (WorldMapPanelHalfSize * DeltaWorldMapZoom);
		}

		TargetWorldMapZoom = FMath::Clamp(TargetWorldMapZoom * DeltaWorldMapZoom, MinWorldMapZoom, MaxWorldMapZoom);
		TargetWorldMapSize = InitialWorldMapSize * TargetWorldMapZoom;
	}
	else
	{
		if (TargetWorldMapZoom > MinWorldMapZoom * DeltaWorldMapZoom)
		{
			FVector2D WorldMapPanelHalfSize = WorldMapPanelSlot->GetSize() / 2.f;
			FVector2D WorldMapPanelPos = WorldMapPanelSlot->GetPosition();
			FVector2D MouseLocalPos = CanvasPanel_WorldMap->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			TargetWorldMapPos = -WorldMapPanelHalfSize + WorldMapPanelPos + (MouseLocalPos / DeltaWorldMapZoom) + (WorldMapPanelHalfSize / DeltaWorldMapZoom);
		}

		TargetWorldMapZoom = FMath::Clamp(TargetWorldMapZoom / DeltaWorldMapZoom, MinWorldMapZoom, MaxWorldMapZoom);
		TargetWorldMapSize = InitialWorldMapSize * TargetWorldMapZoom;
	}
	
	return FReply::Handled();
}

FReply UD1WorldMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (CanvasPanel_WorldMap->IsHovered() && InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		FVector2D MouseLocalPos = CanvasPanel_WorldMap->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		FVector2D WorldMapPanelHalfSize = WorldMapPanelSlot->GetSize() / 2.f;
		FVector2D PinIconSlotPos = MouseLocalPos - WorldMapPanelHalfSize;
		InitialPinIconPos = PinIconSlotPos / GetCurrentWorldMapZoom();
		
		PinIconSlot->SetPosition(PinIconSlotPos);
		Image_PinIcon->SetVisibility(ESlateVisibility::Visible);

		FVector WorldPos = InitialWidgetPosToWorldPos();

		if (MiniMapWidget)
		{
			MiniMapWidget->ShowPinIcon(WorldPos);
		}

		if (CompassBarWidget)
		{
			CompassBarWidget->ShowPinIcon(WorldPos);
		}
		
		return FReply::Handled();
	}

	return Reply;
}

bool UD1WorldMapWidget::PinIconOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		Image_PinIcon->SetVisibility(ESlateVisibility::Hidden);

		if (MiniMapWidget)
		{
			MiniMapWidget->HidePinIcon();
		}

		if (CompassBarWidget)
		{
			CompassBarWidget->HidePinIcon();
		}
		return true;
	}
	return false;
}

float UD1WorldMapWidget::GetCurrentWorldMapZoom()
{
	return (WorldMapPanelSlot->GetSize() / InitialWorldMapSize).X;
}

FVector UD1WorldMapWidget::InitialWidgetPosToWorldPos()
{
	float x = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WidgetFirstPos.Y, WidgetSecondPos.Y),
		FVector2D(WorldFirstPos.X, WorldSecondPos.X),
		InitialPinIconPos.Y);
	
	float y = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WidgetFirstPos.X, WidgetSecondPos.X),
		FVector2D(WorldFirstPos.Y, WorldSecondPos.Y),
		InitialPinIconPos.X);

	return FVector(x, y, 0.f);
}

FVector2D UD1WorldMapWidget::WorldPosToInitialWidgetPos(const FVector& WorldPos)
{
	float x = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.Y, WorldSecondPos.Y),
		FVector2D(WidgetFirstPos.X, WidgetSecondPos.X),
		WorldPos.Y);

	float y = FMath::GetMappedRangeValueUnclamped(
		FVector2D(WorldFirstPos.X, WorldSecondPos.X),
		FVector2D(WidgetFirstPos.Y, WidgetSecondPos.Y),
		WorldPos.X);
	
	return FVector2D(x, y);
}

float UD1WorldMapWidget::WorldLengthToInitialWidgetLength(float WorldLength)
{
	return WorldLength * (SizeConstant / 100.f);
}
