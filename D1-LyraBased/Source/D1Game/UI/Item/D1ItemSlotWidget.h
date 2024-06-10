#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "D1ItemSlotWidget.generated.h"

class UImage;

UCLASS()
class UD1ItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void ChangeSlotState(UImage* TargetImage, ESlotState NewSlotState);
	void ChangeHoverState(UImage* TargetImage, ESlotState NewHoverState);

private:
	UPROPERTY()
	TMap<TObjectPtr<UImage>, ESlotState> ImageToSlotState;
};
