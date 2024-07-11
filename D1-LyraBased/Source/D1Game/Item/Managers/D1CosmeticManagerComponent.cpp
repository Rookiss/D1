#include "D1CosmeticManagerComponent.h"

#include "D1Define.h"
#include "Actors/D1ArmorBase.h"
#include "Character/LyraCharacter.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CosmeticManagerComponent)

UD1CosmeticManagerComponent::UD1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	const int32 ArmorTypeCount = (int32)EArmorType::Count; 

	DefaultMeshes.SetNumZeroed(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		DefaultMeshes[i].ArmorType = (EArmorType)i;
	}

	CosmeticSlots.SetNumZeroed(ArmorTypeCount);
}

void UD1CosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
				
					USkeletalMesh* CosmeticMesh = DefaultMeshes[i].DefaultMesh; 
					SpawnedActor->SetArmorMesh(CosmeticMesh);
				}
			}
		}
	}
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

void UD1CosmeticManagerComponent::SetArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr)
{
	if (ArmorType == EArmorType::Count)
		return;

	if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)ArmorType])
	{
		if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
		{
			if (ArmorMeshPtr.IsNull())
			{
				const FD1CosmeticDefaultMeshEntry& DefaultMeshEntry = DefaultMeshes[(int32)ArmorType];
				CosmeticActor->SetArmorMesh(DefaultMeshEntry.DefaultMesh);
			}
			else
			{
				USkeletalMesh* ArmorMesh = ULyraAssetManager::GetAssetByPath<USkeletalMesh>(ArmorMeshPtr);
				CosmeticActor->SetArmorMesh(ArmorMesh);
			}
		}
	}
}
