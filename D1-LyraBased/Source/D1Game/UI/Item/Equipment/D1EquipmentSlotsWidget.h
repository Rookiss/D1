#pragma once

#include "D1Define.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "D1EquipmentSlotsWidget.generated.h"

class UD1ItemInstance;
class UD1ItemSlotWidget;
class UD1EquipmentSlotWeaponWidget;
class UD1EquipmentSlotArmorWidget;
class UD1EquipmentSlotUtilityWidget;
class UD1EquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FEquipmentInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
};

UCLASS()
class UD1EquipmentSlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	void ConstructUI(FGameplayTag Channel, const FEquipmentInitializeMessage& Message);
	void DestructUI();
	
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance, int32 ItemCount);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotWeaponWidget>> WeaponSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotArmorWidget>> ArmorSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotUtilityWidget>> UtilitySlotWidgets;
	
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;

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

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotUtilityWidget> Equipment_Utility_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotUtilityWidget> Equipment_Utility_Secondary;

private:
	FDelegateHandle DelegateHandle;
	FGameplayMessageListenerHandle ListenerHandle;
};
