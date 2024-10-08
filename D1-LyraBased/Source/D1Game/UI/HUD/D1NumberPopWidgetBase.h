#pragma once

#include "Blueprint/UserWidget.h"
#include "D1NumberPopWidgetBase.generated.h"

UCLASS()
class UD1NumberPopWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1NumberPopWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor);
};
