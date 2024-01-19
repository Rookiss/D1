#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotsWidget.generated.h"

class UCanvasPanel;
class UUniformGridPanel;
class UD1InventorySlotWidget;
class UD1InventoryEntryWidget;
class UD1InventoryManagerComponent;

UCLASS()
class UD1InventorySlotsWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	void UnHoverSlots();
	void FinishDrag();
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 ItemCount);

public:
	FVector2D GetUnitSlotSize() const { return UnitSlotSize; }

private:
	UPROPERTY()
	TSubclassOf<UD1InventorySlotWidget> SlotWidgetClass;

	UPROPERTY()
	TSubclassOf<UD1InventoryEntryWidget> EntryWidgetClass;

private:
	UPROPERTY()
	TArray<TObjectPtr<UD1InventorySlotWidget>> SlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1InventoryEntryWidget>> EntryWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1InventorySlotWidget>> HoveredSlotWidgets;

	UPROPERTY()
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Entries;
	
private:
	FIntPoint InventorySlotCount = FIntPoint::ZeroValue;
	FVector2D UnitSlotSize = FVector2D::ZeroVector;
	FIntPoint PrevToSlotPos = FIntPoint::ZeroValue;
};
