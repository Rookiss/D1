#include "D1ItemManagerComponent.h"

#include "D1EquipmentManagerComponent.h"
#include "D1InventoryManagerComponent.h"
#include "Actors/D1PickupableItemBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/LyraAssetManager.h"

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

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else
	{
		FIntPoint ToItemSlotPos;
		if (ToEquipmentManager->CanSwapEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
			const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);
			
			UD1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
			UD1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
			ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
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

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos);
	if (MovableCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
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

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromInventoryManager, FromItemSlotPos, ToItemSlotPos);
	if (MovableCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
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

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else if (ToEquipmentManager->CanSwapEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
		const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);
		
		UD1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
		UD1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
		FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
	}
}

void UD1ItemManagerComponent::Server_QuickFromInventory_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
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
		int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
		if (MovableCount > 0)
		{
			UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
		}
		else
		{
			if (MyInventoryManager != FromInventoryManager)
			{
				TArray<FIntPoint> OutToItemSlotPoses;
				TArray<int32> OutToItemCounts;
				MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
				if (MovableCount > 0)
				{
					UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
					for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
					{
						MyInventoryManager->AddItem_Unsafe(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
					}
					return;
				}
			}

			FIntPoint ToItemSlotPos;
			if (MyEquipmentManager->CanSwapEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
			{
				const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
				const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);
				
				UD1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
				UD1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
				FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
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
			TArray<FIntPoint> ToItemSlotPoses;
			TArray<int32> ToItemCounts;
			
			int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, ToItemSlotPoses, ToItemCounts);
			if (MovableItemCount > 0)
			{
				UD1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
				for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
				}
			}
		}
	}
}

void UD1ItemManagerComponent::Server_QuickFromEquipment_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
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

	if (IsAllowedComponent(MyInventoryManager) == false || IsAllowedComponent(MyEquipmentManager) == false)
		return;

	if (MyEquipmentManager == FromEquipmentManager)
	{
		TArray<FIntPoint> ToItemSlotPoses;
		TArray<int32> ToItemCounts;
		
		int32 MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
		if (MovableCount > 0)
		{
			UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
			for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
			{
				MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
			}
		}
	}
	else
	{
		EEquipmentSlotType ToEquipmentSlotType;
		int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
		if (MovableCount > 0)
		{
			UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
		}
		else
		{
			TArray<FIntPoint> ToItemSlotPoses;
			TArray<int32> ToItemCounts;

			MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
			if (MovableCount > 0)
			{
				UD1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
				for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
				}
			}
			else
			{
				if (MyEquipmentManager->CanSwapEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
				{
					const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
					const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);
					
					UD1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
					UD1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
					FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
					MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
				}
			}
		}
	}
}

void UD1ItemManagerComponent::Server_DropItemFromInventory_Implementation(UD1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (HasAuthority() == false)
		return;

	if (FromInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false)
		return;

	UD1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
	if (FromItemCount <= 0)
		return;
	
	if (TryDropItem(FromItemInstance, FromItemCount))
	{
		FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
	}
}

void UD1ItemManagerComponent::Server_DropItemFromEquipment_Implementation(UD1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false)
		return;

	UD1ItemInstance* FromItemInstance = FromEquipmentManager->GetItemInstance(FromEquipmentSlotType);
	if (FromItemInstance == nullptr)
		return;

	int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
	if (FromItemCount <= 0)
		return;
	
	if (TryDropItem(FromItemInstance, FromItemCount))
	{
		FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
	}
}

bool UD1ItemManagerComponent::TryPickItem(AD1PickupableItemBase* PickupableItemActor)
{
	if (HasAuthority() == false)
		return false;

	if (IsValid(PickupableItemActor) == false)
		return false;

	UD1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UD1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return false;

	if (IsAllowedComponent(MyInventoryManager) == false || IsAllowedComponent(MyEquipmentManager) == false)
		return false;

	const FD1PickupInfo& PickupInfo = PickupableItemActor->GetPickupInfo();
	const FD1PickupInstance& PickupInstance = PickupInfo.PickupInstance;
	const FD1PickupTemplate& PickupTemplate = PickupInfo.PickupTemplate;

	int32 ItemTemplateID = 0;
	EItemRarity ItemRarity = EItemRarity::Count;
	int32 ItemCount = 0;
	UD1ItemInstance* ItemInstance = nullptr;
	
	if (PickupInstance.ItemInstance)
	{
		if (PickupInstance.ItemCount <= 0)
			return false;

		ItemTemplateID = PickupInstance.ItemInstance->GetItemTemplateID();
		ItemRarity = PickupInstance.ItemInstance->GetItemRarity();
		ItemCount = PickupInstance.ItemCount;
		ItemInstance = PickupInstance.ItemInstance;
	}
	else if (PickupTemplate.ItemTemplateClass)
	{
		if (PickupTemplate.ItemCount <= 0 || PickupTemplate.ItemRarity == EItemRarity::Count)
			return false;
		
		ItemTemplateID = UD1ItemData::Get().FindItemTemplateIDByClass(PickupTemplate.ItemTemplateClass);
		ItemRarity = PickupTemplate.ItemRarity;
		ItemCount = PickupTemplate.ItemCount;
	}

	EEquipmentSlotType ToEquipmentSlotType;
	int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(ItemTemplateID, ItemRarity, ItemCount, ToEquipmentSlotType);
	if (MovableCount == ItemCount)
	{
		if (ItemInstance == nullptr)
		{
			ItemInstance = NewObject<UD1ItemInstance>();
			ItemInstance->Init(ItemTemplateID, ItemRarity);
		}
		
		MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, ItemInstance, MovableCount);
		
		PickupableItemActor->Destroy();
		return true;
	}
	else
	{
		TArray<FIntPoint> ToItemSlotPoses;
		TArray<int32> ToItemCounts;
			
		MovableCount = MyInventoryManager->CanAddItem(ItemTemplateID, ItemRarity, ItemCount, ToItemSlotPoses, ToItemCounts);
		if (MovableCount == ItemCount)
		{
			if (ItemInstance == nullptr)
			{
				ItemInstance = NewObject<UD1ItemInstance>();
				ItemInstance->Init(ItemTemplateID, ItemRarity);
			}
			
			for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
			{
				MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], ItemInstance, ToItemCounts[i]);
			}
			
			PickupableItemActor->Destroy();
			return true;
		}
	}
	
	return false;
}

bool UD1ItemManagerComponent::TryDropItem(UD1ItemInstance* FromItemInstance, int32 FromItemCount)
{
	if (HasAuthority() == false)
		return false;

	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return false;

	AController* Controller = Cast<AController>(GetOwner());
	ACharacter* Character = Controller ? Cast<ACharacter>(Controller->GetPawn()) : Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
		return false;
	
	const float MaxDistance = 100.f;
	const int32 TryCount = 5.f;
	
	float Radius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	TArray<AActor*> ActorsToIgnore = { Character };

	for (int i = 0; i < TryCount; i++)
	{
		FHitResult HitResult;
		FVector2D RandPoint = FMath::RandPointInCircle(MaxDistance);
		FVector TraceLocation = Character->GetCapsuleComponent()->GetComponentLocation() + FVector(RandPoint.X, RandPoint.Y, 0.f);
		
		if (UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), TraceLocation, TraceLocation, Radius, HalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
			continue;
	
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		TSubclassOf<AD1PickupableItemBase> PickupableItemBaseClass = ULyraAssetManager::Get().GetSubclassByName<AD1PickupableItemBase>("PickupableItemBaseClass");
		AD1PickupableItemBase* PickupableItemActor = GetWorld()->SpawnActor<AD1PickupableItemBase>(PickupableItemBaseClass, TraceLocation, FRotator::ZeroRotator, SpawnParameters);
	
		FD1PickupInfo PickupInfo;
		PickupInfo.PickupInstance.ItemInstance = FromItemInstance;
		PickupInfo.PickupInstance.ItemCount = FromItemCount;
		PickupableItemActor->SetPickupInfo(PickupInfo);

		return true;
	}
	
	return false;
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
