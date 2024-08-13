#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "D1InventorySlotsWidget.generated.h"

class UD1InventoryEntryWidget;
class UD1InventorySlotWidget;
class UD1InventoryManagerComponent;
class UUniformGridPanel;
class UCanvasPanel;
class UD1ItemInstance;

USTRUCT(BlueprintType)
struct FInventoryInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManager;
};

UCLASS()
class UD1InventorySlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message);
	void DestructUI();
	
	void UnhoverSlots();
	void FinishDrag();
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UD1ItemInstance* ItemInstance, int32 InItemCount);

public:
	UD1InventoryManagerComponent* GetInventoryManagerComponent() const { return InventoryManager; }

protected:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
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
	TObjectPtr<UD1InventoryManagerComponent> InventoryManager;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Entries;
	
private:
	FDelegateHandle DelegateHandle;
	FIntPoint PrevDragOverSlotPos = FIntPoint(-1, -1);
	FGameplayMessageListenerHandle ListenerHandle;
};
