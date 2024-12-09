#pragma once

#include "Blueprint/UserWidget.h"
#include "D1NumberPopWidget.generated.h"

class UTextBlock;

UCLASS()
class UD1NumberPopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1NumberPopWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Number;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Animation_NumberPop;
};
