#pragma once

#include "UI/Item/D1ItemEntryWidget.h"
#include "D1InventoryEntryWidget.generated.h"

class USizeBox;
class UTextBlock;
class UD1ItemInstance;
class UD1InventorySlotsWidget;

UCLASS()
class UD1InventoryEntryWidget : public UD1ItemEntryWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UD1InventorySlotsWidget* InSlotsWidget, UD1ItemInstance* InItemInstance, int32 InItemCount);
	void RefreshItemCount(int32 NewItemCount);

protected:
	virtual void NativeOnInitialized() override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
	UPROPERTY()
	TObjectPtr<UD1InventorySlotsWidget> SlotsWidget;

	FIntPoint CachedFromSlotPos = FIntPoint::ZeroValue;
	FVector2D CachedDeltaWidgetPos = FVector2D::ZeroVector;

	int32 ItemCount = 0;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;
};
