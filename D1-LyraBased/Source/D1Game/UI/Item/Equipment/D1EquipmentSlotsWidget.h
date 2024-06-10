#pragma once

#include "D1Define.h"
#include "Blueprint/UserWidget.h"
#include "D1EquipmentSlotsWidget.generated.h"

class UD1ItemInstance;
class UD1ItemSlotWidget;
class UD1EquipmentSlotWeaponWidget;
class UD1EquipmentSlotArmorWidget;
class UD1EquipmentManagerComponent;

UCLASS()
class UD1EquipmentSlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotWeaponWidget>> SlotWeaponWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotArmorWidget>> SlotArmorWidgets;

	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWeaponWidget> Equipment_Weapon_Primary;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWeaponWidget> Equipment_Weapon_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotArmorWidget> Equipment_Armor_Head;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotArmorWidget> Equipment_Armor_Chest;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotArmorWidget> Equipment_Armor_Legs;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotArmorWidget> Equipment_Armor_Hand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotArmorWidget> Equipment_Armor_Foot;
};
