#pragma once

#include "Blueprint/DragDropOperation.h"
#include "D1InventoryDragDrop.generated.h"

class UD1InventoryEntryWidget;

UCLASS()
class UD1InventoryDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UD1InventoryDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<UD1InventoryEntryWidget> EntryWidget;
	
	FIntPoint FromPosition = FIntPoint::ZeroValue;
	FIntPoint DeltaPosition = FIntPoint::ZeroValue;
};
