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

	const int32 ArmorTypeCount = (int32)EArmorType::Count; 

	DefaultMeshes.SetNum(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		DefaultMeshes[i].ArmorType = (EArmorType)i;
	}

	SecondaryMeshes.SetNum(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		SecondaryMeshes[i].ArmorType = (EArmorType)i;
	}
}

void UD1CosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Initialize();
}

void UD1CosmeticManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HeadSlot)
	{
		HeadSlot->DestroyComponent();
	}
	
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
		
		SetPrimaryArmorMesh(ArmorType, ArmorFragment->ArmorMesh);
		
		if (ArmorType == EArmorType::Chest)
		{
			if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
			{
				if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(ArmorFragment->bIsFullBody);
				}
			}
		}
		else if (ArmorType == EArmorType::Helmet)
		{
			if (HeadSlot)
			{
				if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(HeadSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(true);
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
					CosmeticActor->ShouldUseSecondaryMesh(false);
				}
			}
		}
		else if (ArmorType == EArmorType::Helmet)
		{
			if (HeadSlot)
			{
				if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(HeadSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(false);
				}
			}
		}

		SetPrimaryArmorMesh(ArmorType, nullptr);
	}
}

void UD1CosmeticManagerComponent::SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr)
{
	if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)ArmorType])
	{
		if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(CosmeticSlot->GetChildActor()))
		{
			if (ArmorMeshPtr.IsNull())
			{
				CosmeticActor->SetPrimaryArmorMesh(nullptr);
			}
			else
			{
				USkeletalMesh* ArmorMesh = ULyraAssetManager::GetAssetByPath<USkeletalMesh>(ArmorMeshPtr);
				CosmeticActor->SetPrimaryArmorMesh(ArmorMesh);
			}
		}
	}
}

void UD1CosmeticManagerComponent::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	if (HeadSlot)
	{
		if (AD1ArmorBase* CosmeticActor = Cast<AD1ArmorBase>(HeadSlot->GetChildActor()))
		{
			OutMeshComponents.Add(CosmeticActor->GetMeshComponent());
		}
	}
	
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

void UD1CosmeticManagerComponent::Initialize()
{
	if (bInitialized)
		return;

	bInitialized = true;

	const int32 ArmorTypeCount = (int32)EArmorType::Count; 
	CosmeticSlots.SetNumZeroed(ArmorTypeCount);
	
	check(CosmeticSlotClass);
	
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsNetMode(NM_DedicatedServer) == false)
		{
			HeadSlot = SpawnCosmeticSlotActor(HeadDefaultMesh, HeadSecondaryMesh);
			
			for (int32 i = 0; i < (int32)EArmorType::Count; i++)
			{
				CosmeticSlots[i] = SpawnCosmeticSlotActor(DefaultMeshes[i].Mesh, SecondaryMeshes[i].Mesh);
			}
		}
	}
}

UChildActorComponent* UD1CosmeticManagerComponent::SpawnCosmeticSlotActor(USkeletalMesh* DefaultMesh, USkeletalMesh* SecondaryMesh)
{
	UChildActorComponent* CosmeticComponent = nullptr;
	
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		USceneComponent* ComponentToAttachTo = Character->GetMesh();
		CosmeticComponent = NewObject<UChildActorComponent>(Character);
		CosmeticComponent->SetupAttachment(ComponentToAttachTo);
		CosmeticComponent->SetChildActorClass(CosmeticSlotClass);
		CosmeticComponent->RegisterComponent();

		if (AD1ArmorBase* SpawnedActor = Cast<AD1ArmorBase>(CosmeticComponent->GetChildActor()))
		{
			if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
			{
				SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
			}
					
			SpawnedActor->InitializeActor(DefaultMesh, SecondaryMesh);
		}
	}
	
	return CosmeticComponent;
}
