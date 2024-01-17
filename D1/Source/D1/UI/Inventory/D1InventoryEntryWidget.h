#pragma once

#include "UI/D1UserWidget.h"
#include "D1InventoryEntryWidget.generated.h"

class UD1InventorySlotsWidget;
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
	
public:
	void Init(UD1InventorySlotsWidget* InSlotsWidget, const FVector2D& NewSize, UD1ItemInstance* NewItemInstance);
	void RefreshItemCount(int32 NewItemCount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1ItemInstance> ItemInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount = 0;

public:
	UPROPERTY()
	TObjectPtr<UD1InventorySlotsWidget> SlotsWidget;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Hover;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;
};
