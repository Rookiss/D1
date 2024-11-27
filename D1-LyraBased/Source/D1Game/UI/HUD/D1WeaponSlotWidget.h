#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "Item/D1ItemInstance.h"
#include "D1WeaponSlotWidget.generated.h"

class UImage;
class UOverlay;
class UTextBlock;
class UCommonVisibilitySwitcher;
class UD1EquipManagerComponent;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1WeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1WeaponSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);
	
public:
	UPROPERTY(EditAnywhere)
	EWeaponSlotType WidgetWeaponSlotType = EWeaponSlotType::Count;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Slots;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SlotNumber;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_OneSlot;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_OneSlot_Count;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Right;

private:
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_ExpandSlot;
	
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_ShowCrossLine;

private:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
};
