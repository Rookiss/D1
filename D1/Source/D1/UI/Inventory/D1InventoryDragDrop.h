#pragma once

#include "Blueprint/DragDropOperation.h"
#include "D1InventoryDragDrop.generated.h"

UCLASS()
class UD1InventoryDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UD1InventoryDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
