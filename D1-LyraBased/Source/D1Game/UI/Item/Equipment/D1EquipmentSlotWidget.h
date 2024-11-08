#pragma once

#include "Blueprint/UserWidget.h"
#include "D1EquipmentSlotWidget.generated.h"

class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded();

protected:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;

protected:
	bool bAlreadyHovered = false;
};
