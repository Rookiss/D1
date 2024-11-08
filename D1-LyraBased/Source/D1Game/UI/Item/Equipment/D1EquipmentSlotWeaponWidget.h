#pragma once

#include "D1Define.h"
#include "D1EquipmentSlotWidget.h"
#include "D1EquipmentSlotWeaponWidget.generated.h"

class UImage;
class UOverlay;
class UCommonVisibilitySwitcher;
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
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded() override;

public:
	void OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UD1ItemInstance* InItemInstance, int32 InItemCount);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentEntryWidget>> EntryWidgets;
	
	UPROPERTY()
	TArray<TObjectPtr<UImage>> SlotRedImages;

	UPROPERTY()
	TArray<TObjectPtr<UImage>> SlotGreenImages;

	UPROPERTY()
	TArray<TObjectPtr<UOverlay>> SlotOverlays;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_WeaponHandEntry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_LeftHandEntry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_RightHandEntry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_TwoHandEntry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red_LeftHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green_LeftHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red_RightHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green_RightHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red_TwoHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green_TwoHand;

private:
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;
};
