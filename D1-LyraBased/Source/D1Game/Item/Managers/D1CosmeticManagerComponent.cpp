#include "D1CosmeticManagerComponent.h"

#include "D1Define.h"
#include "Actors/D1ArmorBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CosmeticManagerComponent)

FD1CosmeticDefaultMeshSet::FD1CosmeticDefaultMeshSet()
{
	const int32 ArmorTypeCount = (int32)EArmorType::Count; 

	DefaultMeshEntries.SetNum(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		DefaultMeshEntries[i].ArmorType = (EArmorType)i;
	}

	SecondaryMeshEntries.SetNum(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		SecondaryMeshEntries[i].ArmorType = (EArmorType)i;
	}
}

UD1CosmeticManagerComponent::UD1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	const int32 SkinTypeCount = (int32)ECharacterSkinType::Count;

	CosmeticDefaultMeshSets.SetNum(SkinTypeCount);
	for (int i = 0; i < SkinTypeCount; i++)
	{
		CosmeticDefaultMeshSets[i].CharacterSkinType = (ECharacterSkinType)i;
	}
}

void UD1CosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeManager();
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
	
	InitializeManager();
	
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
	if (ArmorType == EArmorType::Count)
		return;
	
	InitializeManager();
	
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

void UD1CosmeticManagerComponent::InitializeManager()
{
	if (bInitialized)
		return;

	bInitialized = true;

	const int32 ArmorTypeCount = (int32)EArmorType::Count;
	CosmeticSlots.SetNumZeroed(ArmorTypeCount);
	
	check(CosmeticSlotClass);
	check(CharacterSkinType != ECharacterSkinType::Count);
	
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsNetMode(NM_DedicatedServer) == false)
		{
			const int32 SkinTypeIndex = (int32)CharacterSkinType;
			const FD1CosmeticDefaultMeshSet& CosmeticDefaultMeshSet = CosmeticDefaultMeshSets[SkinTypeIndex];
			
			HeadSlot = SpawnCosmeticSlotActor(CosmeticDefaultMeshSet.HeadDefaultMesh, CosmeticDefaultMeshSet.HeadSecondaryMesh, NAME_None, nullptr);
			
			for (int32 i = 0; i < (int32)EArmorType::Count; i++)
			{
				EArmorType ArmorType = (EArmorType)i;
				FName SkinMaterialSlotName;
				TSoftObjectPtr<UMaterialInterface> SkinMaterial;
				
				if (ArmorType == EArmorType::Helmet || ArmorType == EArmorType::Chest || ArmorType == EArmorType::Hands)
				{
					SkinMaterialSlotName = FName("UpperBody");
					SkinMaterial = CosmeticDefaultMeshSet.UpperBodySkinMaterial;
				}
				else if (ArmorType == EArmorType::Legs || ArmorType == EArmorType::Foot)
				{
					SkinMaterialSlotName = FName("LowerBody");
					SkinMaterial = CosmeticDefaultMeshSet.LowerBodySkinMaterial;
				}
				
				CosmeticSlots[i] = SpawnCosmeticSlotActor(CosmeticDefaultMeshSet.DefaultMeshEntries[i].DefaultMesh, CosmeticDefaultMeshSet.SecondaryMeshEntries[i].DefaultMesh, SkinMaterialSlotName, SkinMaterial);
			}
		}
	}
}

UChildActorComponent* UD1CosmeticManagerComponent::SpawnCosmeticSlotActor(TSoftObjectPtr<USkeletalMesh> InDefaultMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial)
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
			
			SpawnedActor->InitializeActor(InDefaultMesh, InSecondaryMesh, InSkinMaterialSlotName, InSkinMaterial);
		}
	}
	
	return CosmeticComponent;
}
