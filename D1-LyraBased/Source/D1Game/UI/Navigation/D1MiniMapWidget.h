#pragma once

#include "Blueprint/UserWidget.h"
#include "D1MiniMapWidget.generated.h"

UCLASS()
class UD1MiniMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1MiniMapWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
