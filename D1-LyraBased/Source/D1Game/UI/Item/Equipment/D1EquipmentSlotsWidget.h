#pragma once

#include "D1Define.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "D1EquipmentSlotsWidget.generated.h"

class UCommonVisibilitySwitcher;
class UD1ItemInstance;
class UD1ItemSlotWidget;
class UD1EquipmentSlotWeaponWidget;
class UD1EquipmentSlotSingleWidget;
class UD1EquipManagerComponent;
class UD1EquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FEquipmentInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UD1EquipManagerComponent> EquipManager;
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
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);

public:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotWeaponWidget>> WeaponSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotSingleWidget>> ArmorSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UD1EquipmentSlotSingleWidget>> UtilitySlotWidgets;
	
	UPROPERTY()
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UD1EquipManagerComponent> EquipManager;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWeaponWidget> Widget_Weapon_Primary;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotWeaponWidget> Widget_Weapon_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Armor_Head;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Armor_Chest;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Armor_Legs;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Armor_Hand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Armor_Foot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Utility_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Utility_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Utility_Tertiary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UD1EquipmentSlotSingleWidget> Widget_Utility_Quaternary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Weapon_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Weapon_Secondary;

private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
	FGameplayMessageListenerHandle MessageListenerHandle;
};
