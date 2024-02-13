#pragma once

#include "Blueprint/DragDropOperation.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "D1ItemDragDrop.generated.h"

class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;
class UD1InventoryEntryWidget;

UCLASS()
class UD1ItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UD1ItemDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<UD1InventoryEntryWidget> InventoryEntryWidget;

	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> FromInventoryManager;

	FIntPoint FromItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<UD1EquipmentEntryWidget> EquipmentEntryWidget;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> FromEquipmentManager;

	EEquipmentSlotType FromEquipmentSlotType = EEquipmentSlotType::EquipmentSlotCount;

public:
	FVector2D DeltaWidgetPos = FVector2D::ZeroVector;
	int32 ItemID = 0;
};
