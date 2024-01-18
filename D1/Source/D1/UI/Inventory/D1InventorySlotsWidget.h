#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotsWidget.generated.h"

class UD1InventoryManagerComponent;
class UCanvasPanel;
class UD1InventoryEntryWidget;
class UD1InventorySlotWidget;
class UD1InventoryDragWidget;
class UUniformGridPanel;

UCLASS()
class UD1InventorySlotsWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnInventoryEntryChanged(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount);

public:
	const TArray<UD1InventorySlotWidget*> GetSlotWidgets() const { return SlotWidgets; }
	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
	FVector2D GetUnitSlotSize() const { return UnitSlotSize; }

protected:
	UPROPERTY()
	TSubclassOf<UD1InventorySlotWidget> SlotWidgetClass;

	UPROPERTY()
	TSubclassOf<UD1InventoryEntryWidget> EntryWidgetClass;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Entries;

private:
	UPROPERTY()
	TArray<TObjectPtr<UD1InventorySlotWidget>> SlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1InventoryEntryWidget>> EntryWidgets;

private:
	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;
	
	FIntPoint InventorySlotCount = 0;
	FVector2D UnitSlotSize = FVector2D::ZeroVector;
};
