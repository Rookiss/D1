#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventoryDragWidget.generated.h"

UCLASS()
class UD1InventoryDragWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryDragWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
