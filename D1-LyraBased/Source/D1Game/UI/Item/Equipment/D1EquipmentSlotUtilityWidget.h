#pragma once

#include "D1EquipmentSlotWidget.h"
#include "D1EquipmentSlotUtilityWidget.generated.h"

class UD1EquipmentEntryWidget;
class UOverlay;
class UImage;
class UD1ItemInstance;

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
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void CleanUpDrag();
	
public:
	void OnEquipmentEntryChanged(UD1ItemInstance* NewItemInstance, int32 NewItemCount);

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
