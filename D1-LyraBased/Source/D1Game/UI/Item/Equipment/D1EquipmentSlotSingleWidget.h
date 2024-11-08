#pragma once

#include "D1Define.h"
#include "D1EquipmentSlotWidget.h"
#include "D1EquipmentSlotSingleWidget.generated.h"

class UImage;
class UOverlay;
class UD1ItemInstance;
class UD1EquipmentEntryWidget;
class UD1EquipmentManagerComponent;

UENUM()
enum class EEquipmentSingleSlotType : uint8
{
	None,
	Armor,
	Utility,
};

UCLASS()
class UD1EquipmentSlotSingleWidget : public UD1EquipmentSlotWidget
{
	GENERATED_BODY()
	
public:
	UD1EquipmentSlotSingleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EArmorType InArmorType, UD1EquipmentManagerComponent* InEquipmentManager);
	void Init(EUtilitySlotType InUtilitySlotType, UD1EquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded() override;

public:
	void OnEquipmentEntryChanged(UD1ItemInstance* InItemInstance, int32 InItemCount);

private:
	EEquipmentSlotType GetEquipmentSlotType() const;
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> BaseIconTexture;
	
private:
	UPROPERTY()
	TObjectPtr<UD1EquipmentEntryWidget> EntryWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Entry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_BaseIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;

private:
	EEquipmentSingleSlotType EquipmentSingleSlotType = EEquipmentSingleSlotType::None;
	EArmorType ArmorType = EArmorType::Count;
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;
};
