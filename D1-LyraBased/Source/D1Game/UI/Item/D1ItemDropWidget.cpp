#include "D1ItemDropWidget.h"

#include "D1ItemDragDrop.h"
#include "D1ItemEntryWidget.h"
#include "Item/Managers/D1ItemManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemDropWidget)

UD1ItemDropWidget::UD1ItemDropWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool UD1ItemDropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UD1ItemDragDrop* ItemDragDrop = Cast<UD1ItemDragDrop>(InOperation))
	{
		if (UD1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
		{
			FromEntryWidget->RefreshWidgetOpacity(true);
		}

		if (UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>())
		{
			if (ItemDragDrop->FromInventoryManager)
			{
				ItemManager->Server_DropItemFromInventory(ItemDragDrop->FromInventoryManager, ItemDragDrop->FromItemSlotPos);
			}
			else if (ItemDragDrop->FromEquipmentManager)
			{
				ItemManager->Server_DropItemFromEquipment(ItemDragDrop->FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType);
			}
		}
	}

	return true;
}
