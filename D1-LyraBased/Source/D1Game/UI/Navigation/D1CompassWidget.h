#pragma once

#include "Blueprint/UserWidget.h"
#include "D1CompassWidget.generated.h"

class UCanvasPanelSlot;
class UCanvasPanel;
class UTextBlock;

USTRUCT()
struct FD1CompassTextEntry
{
	GENERATED_BODY()

public:
	FD1CompassTextEntry() { }
	
	FD1CompassTextEntry(float InOffsetRatio, UCanvasPanelSlot* InCompassTextSlot, UTextBlock* InCompassText)
		: OffsetRatio(InOffsetRatio)
		, CompassTextSlot(InCompassTextSlot)
		, CompassText(InCompassText) { }
	
public:
	UPROPERTY()
	float OffsetRatio = 0.f;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CompassTextSlot;

	UPROPERTY()
	TObjectPtr<UTextBlock> CompassText;
};

UCLASS()
class UD1CompassWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1CompassWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	float NumberStepDegree = 15.f;

	UPROPERTY(EditAnywhere)
	TArray<FString> Alphabets = { TEXT("N"), TEXT("E"), TEXT("S"), TEXT("W") };
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Text;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Sample_Number;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Sample_Alphabet;
	
private:
	UPROPERTY()
	TArray<FD1CompassTextEntry> CompassTextEntries;
};
