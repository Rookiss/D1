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
	void Init(UD1ItemInstance* InItemInstance, int32 InItemCount, EEquipmentSlotType InEquipmentSlotType, UD1EquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;

	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
};
