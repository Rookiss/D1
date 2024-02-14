#pragma once

#include "UI/D1UserWidget.h"
#include "D1PlayerInventoryWidget.generated.h"

class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;
class UD1EquipmentSlotsWidget;
class UD1InventorySlotsWidget;
class UTextBlock;

UCLASS()
class UD1PlayerInventoryWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1PlayerInventoryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UD1EquipmentManagerComponent* EquipmentManagerComponent, UD1InventoryManagerComponent* InventoryManagerComponent);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemID, int32 OldItemCount, int32 NewItemCount);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotsWidget> EquipmentSlotsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1InventorySlotsWidget> InventorySlotsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Gold;

private:
	int32 CachedGoldCount = 0;
};
