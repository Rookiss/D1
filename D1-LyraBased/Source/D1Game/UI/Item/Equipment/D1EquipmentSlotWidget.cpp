#include "D1EquipmentSlotWidget.h"

#include "D1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotWidget)

UD1EquipmentSlotWidget::UD1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	OnDragEnded();
}

void UD1EquipmentSlotWidget::OnDragEnded()
{
	bAlreadyHovered = false;
}
