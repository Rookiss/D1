#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventoryEntryWidget.generated.h"

class UD1InventorySlotsWidget;
class UD1InventoryDragWidget;
class UTextBlock;
class UImage;
class UD1ItemInstance;
class USizeBox;

UCLASS()
class UD1InventoryEntryWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
public:
	void Init(UD1InventorySlotsWidget* InSlotsWidget, const FVector2D& InWidgetSize, UD1ItemInstance* InItemInstance, int32 InItemCount);
	void RefreshItemCount(int32 NewItemCount);
	void RefreshWidgetOpacity(bool bClearlyVisible);

public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }
	
protected:
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;

private:
	UPROPERTY()
	TObjectPtr<UD1InventorySlotsWidget> SlotsWidget;
	
	UPROPERTY()
	TSubclassOf<UD1InventoryDragWidget> DragWidgetClass;

	FIntPoint CachedFromSlotPos = FIntPoint::ZeroValue;
	FVector2D CachedDeltaWidgetPos = FVector2D::ZeroVector;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Hover;
};
