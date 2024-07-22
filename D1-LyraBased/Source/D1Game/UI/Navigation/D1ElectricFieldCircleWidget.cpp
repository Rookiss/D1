#include "D1ElectricFieldCircleWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricFieldCircleWidget)

UD1ElectricFieldCircleWidget::UD1ElectricFieldCircleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ElectricFieldCircleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CircleDynamicMaterial = Image_ElectricFieldCircle->GetDynamicMaterial();
	CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	
	CircleDynamicMaterial->GetScalarParameterValue(TEXT("OuterCircle"), CachedOuterCircle);
	CircleDynamicMaterial->GetScalarParameterValue(TEXT("InnerCircle"), CachedInnerCircle);
	
	DeltaRadius = FMath::Abs(CachedOuterCircle - CachedInnerCircle);
}

void UD1ElectricFieldCircleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CanvasPanelSlot == nullptr || CircleDynamicMaterial == nullptr)
		return;
	
	float Size = CanvasPanelSlot->GetSize().X;
	float InnerCircle = FMath::Max(0, CachedOuterCircle - (DeltaRadius * Thickness / Size));
	CircleDynamicMaterial->SetScalarParameterValue(TEXT("InnerCircle"), InnerCircle);
}
