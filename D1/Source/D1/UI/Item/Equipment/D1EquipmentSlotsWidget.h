#pragma once

#include "D1Define.h"
#include "UI/D1UserWidget.h"
#include "D1EquipmentSlotsWidget.generated.h"

class UD1EquipmentSlotWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotsWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void Init(UD1EquipmentManagerComponent* InEquipmentManagerComponent);
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotWidget>> SlotWidgets;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_WeaponPrimary_LeftHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_WeaponPrimary_RightHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_WeaponPrimary_TwoHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_WeaponSecondary_LeftHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_WeaponSecondary_RightHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_WeaponSecondary_TwoHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_Armor_Head;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_Armor_Chest;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_Armor_Legs;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_Armor_Hand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWidget> Equipment_Armor_Foot;
};
