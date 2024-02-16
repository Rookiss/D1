#pragma once

#include "D1Define.h"
#include "UI/D1UserWidget.h"
#include "D1EquipmentSlotWeaponWidget.generated.h"

class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;
class UOverlay;
class UImage;

UCLASS()
class UD1EquipmentSlotWeaponWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EWeaponSlotType InWeaponSlotType);
	
protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

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
