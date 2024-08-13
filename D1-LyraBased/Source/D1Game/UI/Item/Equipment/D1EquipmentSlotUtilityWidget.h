#pragma once

#include "D1EquipmentSlotWidget.h"
#include "D1EquipmentSlotUtilityWidget.generated.h"

class UImage;
class UOverlay;
class UD1ItemInstance;
class UD1EquipmentEntryWidget;

UCLASS()
class UD1EquipmentSlotUtilityWidget : public UD1EquipmentSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotUtilityWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EUtilitySlotType InUtilitySlotType, UD1EquipmentManagerComponent* InEquipmentManager);

protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	virtual void FinishDrag() override;
	
public:
	void OnEquipmentEntryChanged(UD1ItemInstance* InItemInstance, int32 InItemCount);

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> UtilityIconTexture;

private:
	UPROPERTY()
	TObjectPtr<UD1EquipmentEntryWidget> EntryWidget;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Slot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

private:
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;
};
