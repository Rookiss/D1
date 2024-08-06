#include "D1CosmeticManagerComponent.h"

#include "D1Define.h"
#include "Actors/D1ArmorBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CosmeticManagerComponent)

UD1CosmeticManagerComponent::UD1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UD1CosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeComponent();
}

void UD1CosmeticManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (UChildActorComponent* CosmeticSlot : CosmeticSlots)
	{
		if (CosmeticSlot)
		{
			CosmeticSlot->DestroyComponent();
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UD1CosmeticManagerComponent::RefreshArmorMesh(EArmorType ArmorType, const UD1ItemFragment_Equippable_Armor* ArmorFragment)
{
	if (ArmorType == EArmorType::Count)
		return;

	if (ArmorFragment)
	{
		if (ArmorFragment == nullptr || ArmorFragment->ArmorType != ArmorType)
			return;
		
		SetArmorMesh(ArmorFragment->ArmorType, ArmorFragment->ArmorMesh);
		
		if (ArmorFragment->ArmorType == EArmorType::Chest && ArmorFragment->bIsFullBody)
		{
			if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
			{
				if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
				{
					CosmeticActor->SetArmorShouldDefault(true);
				}
			}
		}
	}
	else
	{
		if (ArmorType == EArmorType::Chest)
		{
			if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
			{
				if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
				{
					CosmeticActor->SetArmorShouldDefault(false);
				}
			}
		}

		SetArmorMesh(ArmorType, nullptr);
	}
}

void UD1CosmeticManagerComponent::SetArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr)
{
	if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)ArmorType])
	{
		if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
		{
			if (ArmorMeshPtr.IsNull())
			{
				CosmeticActor->SetArmorMesh(nullptr);
			}
			else
			{
				USkeletalMesh* ArmorMesh = ULyraAssetManager::GetAssetByPath<USkeletalMesh>(ArmorMeshPtr);
				CosmeticActor->SetArmorMesh(ArmorMesh);
			}
		}
	}
}

void UD1CosmeticManagerComponent::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	for (UChildActorComponent* CosmeticSlot : CosmeticSlots)
	{
		if (CosmeticSlot)
		{
			if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
			{
				OutMeshComponents.Add(CosmeticActor->GetMeshComponent());
			}
		}
	}
}

void UD1CosmeticManagerComponent::InitializeComponent()
{
	if (bInitialized)
		return;

	bInitialized = true;

	const int32 ArmorTypeCount = (int32)EArmorType::Count; 

	DefaultMeshes.SetNumZeroed(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		DefaultMeshes[i].ArmorType = (EArmorType)i;
	}

	CosmeticSlots.SetNumZeroed(ArmorTypeCount);
	
	check(CosmeticSlotClass);
	
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsNetMode(NM_DedicatedServer) == false)
		{
			for (int32 i = 0; i < (int32)EArmorType::Count; i++)
			{
				USceneComponent* ComponentToAttachTo = Character->GetMesh();
				UChildActorComponent* CosmeticComponent = NewObject<UChildActorComponent>(Character);
				CosmeticComponent->SetupAttachment(ComponentToAttachTo);
				CosmeticComponent->SetChildActorClass(CosmeticSlotClass);
				CosmeticComponent->RegisterComponent();
				CosmeticSlots[i] = CosmeticComponent;

				if (AD1ArmorBase* SpawnedActor = Cast<AD1ArmorBase>(CosmeticComponent->GetChildActor()))
				{
					if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
					{
						SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
					}
					
					SpawnedActor->InitializeActor(DefaultMeshes[i].DefaultMesh);
				}
			}
		}
	}
}
