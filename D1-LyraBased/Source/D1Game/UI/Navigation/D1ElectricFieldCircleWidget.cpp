#include "D1ElectricFieldCircleWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricFieldCircleWidget)

UD1ElectricFieldCircleWidget::UD1ElectricFieldCircleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ElectricFieldCircleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CircleDynamicMaterial = Image_ElectricFieldCircle->GetDynamicMaterial();
	CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
}

void UD1ElectricFieldCircleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CanvasPanelSlot == nullptr || CircleDynamicMaterial == nullptr)
		return;
	
	float OuterCircle;
	CircleDynamicMaterial->GetScalarParameterValue(TEXT("OuterCircle"), OuterCircle);
	
	float InnerCircle;
	CircleDynamicMaterial->GetScalarParameterValue(TEXT("InnerCircle"), InnerCircle);
	
	float Size = CanvasPanelSlot->GetSize().X;
	float DeltaRadius = FMath::Abs(OuterCircle - InnerCircle);
	InnerCircle = FMath::Max(0, DeltaRadius * Thickness / Size);
	CircleDynamicMaterial->SetScalarParameterValue(TEXT("InnerCircle"), InnerCircle);
}
