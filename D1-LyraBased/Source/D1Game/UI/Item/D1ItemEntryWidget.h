#pragma once

#include "Blueprint/UserWidget.h"
#include "D1ItemEntryWidget.generated.h"

class UD1ItemHoverWidget;
class UImage;
class UD1ItemInstance;
class UD1ItemDragWidget;

USTRUCT(BlueprintType)
struct FItemHoverWidgetVisibilityMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bVisible = false;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UD1ItemInstance> ItemInstance;
};

UCLASS()
class UD1ItemEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1ItemEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	void RefreshWidgetOpacity(bool bClearlyVisible);

public:
	void SetItemInstance(UD1ItemInstance* NewItemInstance);
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }

protected:
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;

protected:
	UPROPERTY()
	TSubclassOf<UD1ItemDragWidget> DragWidgetClass;

	UPROPERTY()
	TSubclassOf<UD1ItemHoverWidget> HoverWidgetClass;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Hover;

private:
	UPROPERTY()
	TObjectPtr<UD1ItemHoverWidget> HoverWidget;
};
