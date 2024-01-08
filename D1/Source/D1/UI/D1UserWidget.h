#pragma once

#include "Blueprint/UserWidget.h"
#include "D1UserWidget.generated.h"

UCLASS()
class UD1UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1UserWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
