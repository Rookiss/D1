#pragma once

#include "D1Define.h"
#include "UI/Item/Inventory/D1InventorySlotWidget.h"
#include "D1EquipmentSlotWidget.generated.h"

class UD1ItemInstance;
class UOverlay;
class UD1EquipmentManagerComponent;
class UD1EquipmentEntryWidget;

UCLASS()
class UD1EquipmentSlotWidget : public UD1InventorySlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UD1EquipmentManagerComponent* InEquipmentManagerComponent, EEquipmentSlotType InEquipmentSlotType);
	
protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void FinishDrag();

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnEquipmentEntryChanged(UD1ItemInstance* InItemInstance);
	
private:
	UPROPERTY()
	TSubclassOf<UD1EquipmentEntryWidget> EntryWidgetClass;

	UPROPERTY()
	TObjectPtr<UD1EquipmentEntryWidget> EntryWidget;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;

private:
	bool bAlreadyHovered = false;
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::EquipmentSlotCount;
};
