#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "Item/D1ItemInstance.h"
#include "D1WeaponSlotWidget.generated.h"

class UImage;
class UOverlay;
class UTextBlock;
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

private:
	void SetActiveWidgetIndex(int32 Index);
	int32 GetActiveWidgetIndex() const { return CurrentWidgetIndex; }

private:
	void OnSequenceFinished(class UUMGSequencePlayer& Player);
	
public:
	UPROPERTY(EditAnywhere)
	EWeaponSlotType WidgetWeaponSlotType = EWeaponSlotType::Count;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_OneSlot_Unselected;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_OneSlot_Selected;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_TwoSlot_Unselected;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_TwoSlot_Selected;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_OneSlot_Unselected_Number;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_OneSlot_Selected_Number;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_TwoSlot_Unselected_Number;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_TwoSlot_Selected_Number;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_OneSlot_Unselected;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_OneSlot_Selected;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Unselected_1;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Selected_1;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Unselected_2;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Selected_2;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_OneSlot_Unselected_Count;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_OneSlot_Selected_Count;

private:
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_Highlight_In_OneSlot;
	
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_Highlight_In_TwoSlot;

private:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
	bool bIsSelected = false;

	int32 CurrentWidgetIndex = 0;
	int32 PendingWidgetIndex = 0;
};
