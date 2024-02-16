#pragma once

#include "D1Define.h"
#include "UI/Item/D1ItemSlotWidget.h"
#include "D1EquipmentSlotWeaponWidget.generated.h"

class UD1ItemInstance;
class UImage;
class UOverlay;
class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotWeaponWidget : public UD1ItemSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EWeaponSlotType InWeaponSlotType);
	
protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void FinishDrag();
	void OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UD1ItemInstance* NewItemInstance);
	EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType) const;
	
private:
	UPROPERTY()
	TSubclassOf<UD1EquipmentEntryWidget> EntryWidgetClass;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentEntryWidget>> EntryWidgets;
	
	UPROPERTY()
	TArray<TObjectPtr<UImage>> SlotImages;

	UPROPERTY()
	TArray<TObjectPtr<UOverlay>> SlotOverlays;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot_Right;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot_Two;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot_Right;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot_Two;

private:
	bool bAlreadyHovered = false;
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;
};
