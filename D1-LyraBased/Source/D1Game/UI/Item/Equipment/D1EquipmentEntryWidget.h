#pragma once

#include "D1Define.h"
#include "UI/Item/D1ItemEntryWidget.h"
#include "D1EquipmentEntryWidget.generated.h"

class UD1ItemInstance;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentEntryWidget : public UD1ItemEntryWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UD1ItemInstance* InItemInstance, EEquipmentSlotType InEquipmentSlotType);
	
protected:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

protected:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;

	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
};
