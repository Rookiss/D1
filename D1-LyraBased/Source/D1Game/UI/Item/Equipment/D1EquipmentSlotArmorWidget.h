#pragma once

#include "D1Define.h"
#include "UI/Item/D1ItemSlotWidget.h"
#include "D1EquipmentSlotArmorWidget.generated.h"

class UImage;
class UOverlay;
class UD1ItemInstance;
class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotArmorWidget : public UD1ItemSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotArmorWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EArmorType InArmorType);
	
protected:
	virtual void NativeOnInitialized() override;
	
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void CleanUpDrag();

public:
	void OnEquipmentEntryChanged(UD1ItemInstance* NewItemInstance);
	
private:
	UPROPERTY()
	TObjectPtr<UD1EquipmentEntryWidget> EntryWidget;
	
	UPROPERTY()
	TSubclassOf<UD1EquipmentEntryWidget> EntryWidgetClass;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot;

private:
	bool bAlreadyHovered = false;
	EArmorType ArmorType = EArmorType::Count;
};
