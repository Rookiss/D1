#include "D1EquipmentSlotWidget.h"

#include "System/LyraAssetManager.h"
#include "D1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotWidget)

UD1EquipmentSlotWidget::UD1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EntryWidgetClass = ULyraAssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidgetClass");
}

void UD1EquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	FinishDrag();
}

void UD1EquipmentSlotWidget::FinishDrag()
{
	bAlreadyHovered = false;
}
