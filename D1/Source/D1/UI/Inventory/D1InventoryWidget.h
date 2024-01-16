#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventoryWidget.generated.h"

UCLASS()
class UD1InventoryWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
