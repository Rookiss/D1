#include "D1ItemHoverWidget.h"

#include "D1ItemHoverEntryWidget.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemHoverWidget)

UD1ItemHoverWidget::UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemHoverWidget::RefreshUI(const UD1ItemInstance* HoveredItemInstance)
{
	const UD1ItemInstance* PairItemInstance = nullptr;
	if (UD1EquipmentManagerComponent* EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipmentManagerComponent>())
	{
		EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
		PairItemInstance = EquipmentManager->FindPairItemInstance(HoveredItemInstance, EquipmentSlotType);
	}
	
	HoverWidget_Left->RefreshUI(HoveredItemInstance);
	HoverWidget_Right->RefreshUI(PairItemInstance);
}
