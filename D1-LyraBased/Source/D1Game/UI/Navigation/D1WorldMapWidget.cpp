#include "D1WorldMapWidget.h"

#include "D1CompassWidget.h"
#include "D1ElectricFieldCircleWidget.h"
#include "D1MiniMapWidget.h"
#include "Actors/D1ElectricField.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/LyraGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LyraPlayerController.h"
#include "System/D1ElectricFieldManagerComponent.h"

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

	MaxWorldMapZoom = MinWorldMapZoom * FMath::Pow(MultiplierWorldMapZoom, StepWorldMapZoom - 1);
	WidgetUnitSize = (WorldPosToInitialWidgetPos(FVector(100.f, 0.f, 0.f)) - WorldPosToInitialWidgetPos(FVector(0.f, 0.f, 0.f))).Length();

	CurrentCircleWidget->SetVisibility(ESlateVisibility::Hidden);
	TargetCircleWidget->SetVisibility(ESlateVisibility::Hidden);

	MultiplierWorldMapZoom = FMath::Max(1.f, MultiplierWorldMapZoom);
}

void UD1WorldMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (FMath::IsNearlyEqual(TargetWorldMapZoom, MinWorldMapZoom))
	{
		// Reset World Map Panel Target Position
		TargetWorldMapPos = FVector2D::ZeroVector;
	}
	
	// Refresh World Map Panel Size & Position
	FVector2D NewSize = FMath::Vector2DInterpTo(WorldMapPanelSlot->GetSize(), TargetWorldMapSize, InDeltaTime, InitialInterpSpeed);
	WorldMapPanelSlot->SetSize(NewSize);

	FVector2D NewPos = FMath::Vector2DInterpTo(WorldMapPanelSlot->GetPosition(), TargetWorldMapPos, InDeltaTime, InitialInterpSpeed);
	WorldMapPanelSlot->SetPosition(NewPos);
	
	// Clamp World Map Panel Position
	FVector2D HalfSize = WorldMapPanelSlot->GetSize() / 2.f;
	FVector2D WorldMapPos = WorldMapPanelSlot->GetPosition();
	float x = FMath::Clamp(WorldMapPos.X, -(HalfSize.X - InitialWorldMapSize.X / 2.f), +(HalfSize.X - InitialWorldMapSize.X / 2.f));
	float y = FMath::Clamp(WorldMapPos.Y, -(HalfSize.Y - InitialWorldMapSize.Y / 2.f), +(HalfSize.Y - InitialWorldMapSize.Y / 2.f));
	WorldMapPanelSlot->SetPosition(FVector2D(x, y));

	// Refresh Player Panel Position/Rotation & Pin Icon Position
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		FVector2D InitialPos = WorldPosToInitialWidgetPos(Pawn->GetActorLocation());
		PlayerPanelSlot->SetPosition(InitialPos * GetCurrentWorldMapZoom());
	}

	if (ALyraPlayerController* ControllerOwner = Cast<ALyraPlayerController>(GetOwningPlayer()))
	{
		if (APlayerCameraManager* CameraManager = ControllerOwner->PlayerCameraManager)
		{
			CanvasPanel_Player->SetRenderTransformAngle(CameraManager->GetCameraRotation().Yaw);
		}
	}

	PinIconSlot->SetPosition(InitialPinIconPos * GetCurrentWorldMapZoom());

	// Refresh Circle Widget
	if (AD1ElectricField* ElectricFieldActor = GetElectricFieldActor())
	{
		CurrentCircleWidget->SetVisibility(ESlateVisibility::Visible);
		
		FVector2D InitialPos = WorldPosToInitialWidgetPos(ElectricFieldActor->GetActorLocation());
		CurrCircleSlot->SetPosition(InitialPos * GetCurrentWorldMapZoom());
			
		FVector2D InitialSize = FVector2D(WorldLengthToInitialWidgetLength(ElectricFieldActor->GetActorScale3D().X * 50.f * 2.f));
		CurrCircleSlot->SetSize(InitialSize * GetCurrentWorldMapZoom());

		if (ALyraGameState* LyraGameState = Cast<ALyraGameState>(UGameplayStatics::GetGameState(GetOwningPlayer())))
		{
			if (UD1ElectricFieldManagerComponent* ElectricFieldManager = LyraGameState->FindComponentByClass<UD1ElectricFieldManagerComponent>())
			{
				TargetCircleWidget->SetVisibility(ElectricFieldManager->bShouldShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			
				InitialPos = WorldPosToInitialWidgetPos(ElectricFieldManager->TargetPhasePosition);
				TargetCircleSlot->SetPosition(InitialPos * GetCurrentWorldMapZoom());
				
				InitialSize = InitialSize = FVector2D(WorldLengthToInitialWidgetLength(ElectricFieldManager->TargetPhaseRadius * 2.f));
				TargetCircleSlot->SetSize(InitialSize * GetCurrentWorldMapZoom());
			}
		}
	}
	else
	{
		CurrentCircleWidget->SetVisibility(ESlateVisibility::Hidden);
		TargetCircleWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UD1WorldMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && TargetWorldMapZoom > MinWorldMapZoom)
	{
		FVector2D AbsoluteCursorDelta = InMouseEvent.GetCursorDelta();
		FVector2D AbsolutePanelPos = USlateBlueprintLibrary::LocalToAbsolute(CanvasPanel_WorldMap->GetCachedGeometry(), WorldMapPanelSlot->GetPosition());
		TargetWorldMapPos = CanvasPanel_WorldMap->GetCachedGeometry().AbsoluteToLocal(AbsolutePanelPos + AbsoluteCursorDelta);
		WorldMapPanelSlot->SetPosition(TargetWorldMapPos);
		return FReply::Handled();
	}
	
	return Reply;
}

FReply UD1WorldMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseWheel(InGeometry, InMouseEvent);

	if (FMath::Abs(TargetWorldMapZoom - GetCurrentWorldMapZoom()) > 0.1f)
		return Reply;
	
	if (InMouseEvent.GetWheelDelta() > 0.f)
	{
		if (TargetWorldMapZoom < MaxWorldMapZoom)
		{
			FVector2D WorldMapPanelHalfSize = WorldMapPanelSlot->GetSize() / 2.f;
			FVector2D WorldMapPanelPos = WorldMapPanelSlot->GetPosition();
			FVector2D MouseLocalPos = CanvasPanel_WorldMap->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			TargetWorldMapPos = (WorldMapPanelPos - WorldMapPanelHalfSize) - (MouseLocalPos * (MultiplierWorldMapZoom - 1)) + (WorldMapPanelHalfSize * MultiplierWorldMapZoom);
		}

		TargetWorldMapZoom = FMath::Clamp(TargetWorldMapZoom * MultiplierWorldMapZoom, MinWorldMapZoom, MaxWorldMapZoom);
		TargetWorldMapSize = InitialWorldMapSize * TargetWorldMapZoom;
	}
	else
	{
		if (TargetWorldMapZoom > MinWorldMapZoom * MultiplierWorldMapZoom)
		{
			FVector2D WorldMapPanelHalfSize = WorldMapPanelSlot->GetSize() / 2.f;
			FVector2D WorldMapPanelPos = WorldMapPanelSlot->GetPosition();
			FVector2D MouseLocalPos = CanvasPanel_WorldMap->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			TargetWorldMapPos = (WorldMapPanelPos - WorldMapPanelHalfSize) + (MouseLocalPos / MultiplierWorldMapZoom * (MultiplierWorldMapZoom - 1)) + (WorldMapPanelHalfSize / MultiplierWorldMapZoom);
		}

		TargetWorldMapZoom = FMath::Clamp(TargetWorldMapZoom / MultiplierWorldMapZoom, MinWorldMapZoom, MaxWorldMapZoom);
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

		if (CompassWidget)
		{
			CompassWidget->ShowPinIcon(WorldPos);
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

		if (CompassWidget)
		{
			CompassWidget->HidePinIcon();
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
	return WorldLength * (WidgetUnitSize / 100.f);
}

AD1ElectricField* UD1WorldMapWidget::GetElectricFieldActor()
{
	AD1ElectricField* ElectricFieldActor = nullptr;

	if (ALyraGameState* LyraGameState = Cast<ALyraGameState>(UGameplayStatics::GetGameState(GetOwningPlayer())))
	{
		if (UD1ElectricFieldManagerComponent* ElectricFieldManager = LyraGameState->FindComponentByClass<UD1ElectricFieldManagerComponent>())
		{
			ElectricFieldActor = ElectricFieldManager->ElectricFieldActor;
		}
	}
	
	return ElectricFieldActor;
}
