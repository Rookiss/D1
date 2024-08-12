#pragma once

#include "D1Define.h"
#include "D1EquipmentSlotWidget.h"
#include "D1EquipmentSlotWeaponWidget.generated.h"

class UImage;
class UOverlay;
class UD1ItemInstance;
class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotWeaponWidget : public UD1EquipmentSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EWeaponSlotType InWeaponSlotType, UD1EquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	virtual void FinishDrag() override;

public:
	void OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UD1ItemInstance* InItemInstance, int32 InItemCount);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentEntryWidget>> EntryWidgets;
	
	UPROPERTY()
	TArray<TObjectPtr<UImage>> SlotImages;

	UPROPERTY()
	TArray<TObjectPtr<UOverlay>> SlotOverlays;

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> WeaponIconTexture_Left;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> WeaponIconTexture_Right;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot_LeftHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot_RightHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot_TwoHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot_LeftHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot_RightHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot_TwoHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon_Left;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon_Right;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Frame_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Frame_Right;

private:
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;
};
