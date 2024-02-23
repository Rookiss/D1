#pragma once

#include "UI/D1UserWidget.h"
#include "D1PlayerInventoryWidget.generated.h"

class UTextBlock;
class UD1ItemInstance;
class UD1EquipmentSlotsWidget;
class UD1InventorySlotsWidget;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS()
class UD1PlayerInventoryWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1PlayerInventoryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void NativeConstruct() override;

private:
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 NewItemCount);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotsWidget> EquipmentSlotsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1InventorySlotsWidget> InventorySlotsWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Gold;

private:
	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;
};
