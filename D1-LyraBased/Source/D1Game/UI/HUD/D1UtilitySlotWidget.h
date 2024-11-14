#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "D1UtilitySlotWidget.generated.h"

class UD1ItemInstance;
class UTextBlock;
class UImage;
class UD1EquipmentManagerComponent;
class UD1EquipManagerComponent;

UCLASS()
class UD1UtilitySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1UtilitySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);
	
public:
	UPROPERTY(EditAnywhere)
	EUtilitySlotType WidgetUtilitySlotType = EUtilitySlotType::Count;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SlotNumber;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Highlight;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_Highlight_In;
	
private:
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
};
