#include "D1CompassWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CompassWidget)

UD1CompassWidget::UD1CompassWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CompassWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	float NumberStepCount = 360.f / NumberStepDegree;
	float NumberIntervalRatio = NumberStepDegree / 360.f;

	float AlphabetStepCount = Alphabets.Num();
	float AlphabetStepDegree = 360.f / AlphabetStepCount;
	float AlphabetIntervalRatio = AlphabetStepDegree / 360.f;
	
	UCanvasPanelSlot* SampleNumberSlot = Cast<UCanvasPanelSlot>(Text_Sample_Number->Slot);
	UCanvasPanelSlot* SampleAlphabetSlot = Cast<UCanvasPanelSlot>(Text_Sample_Alphabet->Slot);
	
	for (int32 i = 0; i < NumberStepCount; i++)
	{
		if (FMath::IsNearlyEqual(UKismetMathLibrary::GenericPercent_FloatFloat(i * NumberStepDegree, AlphabetStepDegree), 0.f))
			continue;
		
		UTextBlock* NewText = DuplicateObject<UTextBlock>(Text_Sample_Number, this);
		NewText->SetText(FText::AsNumber(i * NumberStepDegree));

		UCanvasPanelSlot* NewTextSlot = CanvasPanel_Text->AddChildToCanvas(NewText);
		NewTextSlot->SetLayout(SampleNumberSlot->GetLayout());
		NewTextSlot->SetAutoSize(SampleNumberSlot->GetAutoSize());
		CompassTextEntries.Emplace(i * NumberIntervalRatio, NewTextSlot, NewText);
	}

	for (int32 i = 0; i < AlphabetStepCount; i++)
	{
		UTextBlock* NewText = DuplicateObject<UTextBlock>(Text_Sample_Alphabet, this);
		NewText->SetText(FText::FromString(Alphabets[i]));

		UCanvasPanelSlot* NewTextSlot = CanvasPanel_Text->AddChildToCanvas(NewText);
		NewTextSlot->SetLayout(SampleAlphabetSlot->GetLayout());
		NewTextSlot->SetAutoSize(SampleAlphabetSlot->GetAutoSize());
		CompassTextEntries.Emplace(i * AlphabetIntervalRatio, NewTextSlot, NewText);
	}
}

void UD1CompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerCameraManager* CameraManager = GetOwningPlayerCameraManager();
	if (CameraManager == nullptr)
		return;

	float CameraYaw = CameraManager->GetCameraRotation().Yaw;
	float CompassRatio = -(CameraYaw / 360.f);
	
	FVector2D CanvasLocalSize = CanvasPanel_Text->GetCachedGeometry().GetLocalSize();

	for (const FD1CompassTextEntry& CompassTextEntry : CompassTextEntries)
	{
		float FinalRatio = UKismetMathLibrary::GenericPercent_FloatFloat(1.5f + CompassRatio + CompassTextEntry.OffsetRatio, 1.f);
		CompassTextEntry.CompassTextSlot->SetPosition(FVector2D(CanvasLocalSize.X * FinalRatio, 0.f));
		CompassTextEntry.CompassText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UD1CompassWidget::ShowPinIcon(const FVector& WorldPos)
{
	
}

void UD1CompassWidget::HidePinIcon()
{
	
}
