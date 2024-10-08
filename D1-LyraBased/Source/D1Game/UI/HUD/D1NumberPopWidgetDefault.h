#pragma once

#include "D1NumberPopWidgetBase.h"
#include "D1NumberPopWidgetDefault.generated.h"

class UTextBlock;

UCLASS()
class UD1NumberPopWidgetDefault : public UD1NumberPopWidgetBase
{
	GENERATED_BODY()
	
public:
	UD1NumberPopWidgetDefault(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Number;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Animation_NumberPop;
};
