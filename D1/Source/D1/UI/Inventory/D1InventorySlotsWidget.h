#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventorySlotsWidget.generated.h"

class UD1InventorySlotWidget;
class UUniformGridPanel;

UCLASS()
class UD1InventorySlotsWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnInventoryEntryChanged(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount);

protected:
	UPROPERTY(BlueprintReadOnly)
	FIntPoint SlotCount = 0;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UD1InventorySlotWidget> SlotWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UD1InventorySlotWidget>> SlotWidgets;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;
};
