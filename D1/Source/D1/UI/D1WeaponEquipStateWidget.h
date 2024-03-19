#pragma once

#include "D1Define.h"
#include "D1UserWidget.h"
#include "D1WeaponEquipStateWidget.generated.h"

class UOverlay;
class UD1ItemInstance;

UCLASS()
class UD1WeaponEquipStateWidget : public UD1UserWidget
{
	GENERATED_BODY()
	
public:
	UD1WeaponEquipStateWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance);
	void OnWeaponEquipStateChanged(EWeaponEquipState WeaponEquipState);
	
protected:
	TArray<TObjectPtr<UOverlay>> WeaponOverlays;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Primary_LeftHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Primary_RightHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Primary_TwoHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Secondary_LeftHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Secondary_RightHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Secondary_TwoHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_QuickSlot_1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_QuickSlot_2;
};
