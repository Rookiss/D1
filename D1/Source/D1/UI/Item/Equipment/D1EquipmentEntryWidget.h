#pragma once

#include "D1Define.h"
#include "UI/D1UserWidget.h"
#include "D1EquipmentEntryWidget.generated.h"

class UImage;
class USizeBox;
class UD1ItemInstance;
class UD1ItemDragWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentEntryWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UD1ItemInstance* InItemInstance, EEquipmentSlotType InEquipmentSlotType);
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	void RefreshWidgetOpacity(bool bClearlyVisible);
	
public:
	UD1ItemInstance* GetItemInstance() const { return ItemInstance; }

protected:
	UPROPERTY()
	TObjectPtr<UD1ItemInstance> ItemInstance;
	
	UPROPERTY()
	TSubclassOf<UD1ItemDragWidget> DragWidgetClass;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;

	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::EquipmentSlotCount;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Hover;
};
