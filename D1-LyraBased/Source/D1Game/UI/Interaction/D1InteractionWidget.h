#pragma once

#include "Blueprint/UserWidget.h"
#include "D1InteractionWidget.generated.h"

UCLASS()
class UD1InteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1InteractionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
