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
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnInventoryEntryChanged(const FIntPoint& Position, UD1ItemInstance* Instance, int32 ItemCount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntPoint SlotCount;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UD1InventorySlotWidget> SlotWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UD1InventorySlotWidget>> SlotWidgets;

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;
};
