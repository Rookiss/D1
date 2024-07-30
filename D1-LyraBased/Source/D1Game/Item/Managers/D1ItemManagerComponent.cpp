#include "D1ItemManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
#include "D1InventoryManagerComponent.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemManagerComponent)

UD1ItemManagerComponent::UD1ItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UD1ItemManagerComponent::Server_InventoryToEquipment_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;
	
	if (ToEquipmentManager->CanAddEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, 1);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
}

void UD1ItemManagerComponent::Server_EquipmentToInventory_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	if (ToInventoryManager->CanMoveOrMergeItem(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos))
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
		ToInventoryManager->AddItem(ToItemSlotPos, RemovedItemInstance, 1);
	}
}

void UD1ItemManagerComponent::Server_InventoryToInventory_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UD1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	int32 MovableItemCount = ToInventoryManager->CanMoveOrMergeItem(FromInventoryManager, FromItemSlotPos, ToItemSlotPos);
	if (MovableItemCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, MovableItemCount);
		ToInventoryManager->AddItem(ToItemSlotPos, RemovedItemInstance, MovableItemCount);
	}
}

void UD1ItemManagerComponent::Server_EquipmentToEquipment_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UD1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;
	
	if (ToEquipmentManager->CanAddEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
		ToEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
	}
}

void UD1ItemManagerComponent::Server_MoveQuickFromInventory_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false)
		return;
	
	UD1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UD1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(MyInventoryManager) == false || IsAllowedComponent(MyEquipmentManager) == false)
		return;

	UD1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	if (FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>())
	{
		// 1. [장비]
		// 1-1. [내 인벤토리] -> 내 장비 장착 -> 내 장비 교체
		// 1-2. [다른 인벤토리] -> 내 장비 장착 -> 내 인벤토리 -> 내 장비 교체
	
		EEquipmentSlotType ToEquipmentSlotType;
		if (MyEquipmentManager->CanAddEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType))
		{
			UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, 1);
			MyEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
		}
		else
		{
			if (MyInventoryManager != FromInventoryManager)
			{
				TArray<FIntPoint> OutToItemSlotPoses;
				TArray<int32> OutToItemCounts;
				int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
				if (MovableItemCount > 0)
				{
					UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, MovableItemCount);
					for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
					{
						MyInventoryManager->AddItem(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
					}
					return;
				}
			}
		
			if (MyEquipmentManager->CanAddEquipment_Swap(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType))
			{
				UD1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment(ToEquipmentSlotType);
				UD1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem(FromItemSlotPos, 1);
				MyEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstanceFrom);
				FromInventoryManager->AddItem(FromItemSlotPos, RemovedItemInstanceTo, 1);
			}
		}
	}
	else
	{
		// 2. [일반 아이템]
		// 2-1. [내 인벤토리] -> X
		// 2-2. [다른 인벤토리] -> 내 인벤토리

		if (MyInventoryManager != FromInventoryManager)
		{
			TArray<FIntPoint> OutToItemSlotPoses;
			TArray<int32> OutToItemCounts;
			int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
			if (MovableItemCount > 0)
			{
				UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem(FromItemSlotPos, MovableItemCount);
				for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
				}
			}
		}
	}
}

void UD1ItemManagerComponent::Server_MoveQuickFromEquipment_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false)
		return;

	// 1. [내 장비창] -> 내 인벤토리
	// 2. [다른 장비창] -> 내 장비 장착 -> 내 인벤토리 -> 내 장비 교체

	UD1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UD1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(MyEquipmentManager) == false)
		return;

	if (MyEquipmentManager == FromEquipmentManager)
	{
		FIntPoint ToItemSlotPos;
		if (MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos))
		{
			UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
			MyInventoryManager->AddItem(ToItemSlotPos, RemovedItemInstance, 1);
		}
	}
	else
	{
		EEquipmentSlotType ToEquipmentSlotType;
		if (MyEquipmentManager->CanAddEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
		{
			UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
			MyEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstance);
		}
		else
		{
			FIntPoint OutToItemSlotPos;
			if (MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, OutToItemSlotPos))
			{
				UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
				MyInventoryManager->AddItem(OutToItemSlotPos, RemovedItemInstance, 1);
			}
			else if (MyEquipmentManager->CanAddEquipment_Swap(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
			{
				UD1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment(ToEquipmentSlotType);
				UD1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment(FromEquipmentSlotType);
				MyEquipmentManager->AddEquipment(ToEquipmentSlotType, RemovedItemInstanceFrom);
				FromEquipmentManager->AddEquipment(FromEquipmentSlotType, RemovedItemInstanceTo);
			}
		}
	}
}

void UD1ItemManagerComponent::AddAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Add(ActorComponent);
}

void UD1ItemManagerComponent::RemoveAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Remove(ActorComponent);
}

bool UD1ItemManagerComponent::IsAllowedComponent(UActorComponent* ActorComponent) const
{
	return AllowedComponents.Contains(ActorComponent);
}

UD1InventoryManagerComponent* UD1ItemManagerComponent::GetMyInventoryManager() const
{
	UD1InventoryManagerComponent* MyInventoryManager = nullptr;
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyInventoryManager = Pawn->GetComponentByClass<UD1InventoryManagerComponent>();
		}
	}

	return MyInventoryManager;
}

UD1EquipmentManagerComponent* UD1ItemManagerComponent::GetMyEquipmentManager() const
{
	UD1EquipmentManagerComponent* MyEquipmentManager = nullptr;
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyEquipmentManager = Pawn->GetComponentByClass<UD1EquipmentManagerComponent>();
		}
	}

	return MyEquipmentManager;
}
