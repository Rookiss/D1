#pragma once

#include "UI/Item/D1ItemSlotWidget.h"
#include "D1EquipmentSlotWidget.generated.h"

class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotWidget : public UD1ItemSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
protected:
	virtual void FinishDrag();
	
protected:
	UPROPERTY()
	TSubclassOf<UD1EquipmentEntryWidget> EntryWidgetClass;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;

protected:
	bool bAlreadyHovered = false;
};
