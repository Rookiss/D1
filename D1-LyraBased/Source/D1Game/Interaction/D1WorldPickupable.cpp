#include "D1WorldPickupable.h"

#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldPickupable)

AD1WorldPickupable::AD1WorldPickupable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

void AD1WorldPickupable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PickupInfo);
}

void AD1WorldPickupable::OnRep_PickupInfo()
{
	if (const UD1ItemInstance* ItemInstance = PickupInfo.PickupInstance.ItemInstance)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		InteractionInfo.Content = ItemTemplate.DisplayName;
	}
	else if (TSubclassOf<UD1ItemTemplate> ItemTemplateClass = PickupInfo.PickupTemplate.ItemTemplateClass)
	{
		if (const UD1ItemTemplate* ItemTemplate = ItemTemplateClass->GetDefaultObject<UD1ItemTemplate>())
		{
			InteractionInfo.Content = ItemTemplate->DisplayName;
		}
	}
}

void AD1WorldPickupable::SetPickupInfo(const FD1PickupInfo& InPickupInfo)
{
	if (HasAuthority() == false)
		return;
	
	if (InPickupInfo.PickupInstance.ItemInstance || InPickupInfo.PickupTemplate.ItemTemplateClass)
	{
		PickupInfo = InPickupInfo;
		OnRep_PickupInfo();
	}
	else
	{
		Destroy();
	}
}
