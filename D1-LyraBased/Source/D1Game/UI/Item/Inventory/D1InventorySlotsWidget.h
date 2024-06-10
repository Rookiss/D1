#pragma once

#include "Blueprint/UserWidget.h"
#include "D1InventorySlotsWidget.generated.h"

class UD1InventoryEntryWidget;
class UD1InventorySlotWidget;
class UD1InventoryManagerComponent;
class UUniformGridPanel;
class UCanvasPanel;
class UD1ItemInstance;

UCLASS()
class UD1InventorySlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void UnhoverSlots();
	void CleanUpDrag();
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 NewItemCount);

public:
	UD1InventoryManagerComponent* GetInventoryManagerComponent() const { return InventoryManagerComponent; }

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
	FIntPoint PrevDragOverSlotPos = FIntPoint(-1, -1);
};
