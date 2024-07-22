#include "D1CompassBarWidget.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CompassBarWidget)

UD1CompassBarWidget::UD1CompassBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CompassBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Image_PinIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UD1CompassBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PinIconDynamicMaterial = Image_PinIcon->GetDynamicMaterial();
	CompassDynamicMaterial = Image_CompassBar->GetDynamicMaterial();
}

void UD1CompassBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager == nullptr)
		return;
	
	float Yaw = CameraManager->GetCameraRotation().Yaw / 360.f;
	CompassDynamicMaterial->SetScalarParameterValue(TEXT("Yaw"), Yaw);

	if (bIsTargetSet)
	{
		APawn* Pawn = GetOwningPlayerPawn();
		if (IsValid(Pawn) == false)
			return;
		
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), TargetWorldPos);
		PinIconDynamicMaterial->SetScalarParameterValue("Yaw", Yaw - Rotation.Yaw / 360.f);
	}
}

void UD1CompassBarWidget::ShowPinIcon(const FVector& WorldPos)
{
	bIsTargetSet = true;
	TargetWorldPos = WorldPos;
	Image_PinIcon->SetVisibility(ESlateVisibility::Visible);
}

void UD1CompassBarWidget::HidePinIcon()
{
	bIsTargetSet = false;
	TargetWorldPos = FVector::ZeroVector;
	Image_PinIcon->SetVisibility(ESlateVisibility::Hidden);
}
