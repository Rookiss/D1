#pragma once

#include "D1Define.h"
#include "Blueprint/DragDropOperation.h"
#include "D1ItemDragDrop.generated.h"

class UD1ItemInstance;
class UD1ItemEntryWidget;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS()
class UD1ItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UD1ItemDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> FromInventoryManager;

	FIntPoint FromItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> FromEquipmentManager;

	EEquipmentSlotType FromEquipmentSlotType = EEquipmentSlotType::Count;

public:
	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> ToInventoryManager;

	FIntPoint ToItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> ToEquipmentManager;

	EEquipmentSlotType ToEquipmentSlotType = EEquipmentSlotType::Count;
	
public:
	UPROPERTY()
	TObjectPtr<UD1ItemEntryWidget> FromEntryWidget;
	
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> FromItemInstance;
	
	FVector2D DeltaWidgetPos = FVector2D::ZeroVector;
};
