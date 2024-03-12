#include "D1WeaponBase.h"

#include "Character/D1Player.h"
#include "Components/BoxComponent.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WeaponBase)

AD1WeaponBase::AD1WeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionProfileName("Weapon");
	WeaponMesh->SetGenerateOverlapEvents(true);

	DebugCollision = CreateDefaultSubobject<UBoxComponent>("DebugCollision");
	DebugCollision->SetupAttachment(GetRootComponent());
	DebugCollision->SetCollisionProfileName("NoCollision");
}

void AD1WeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, EquipmentSlotType, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bCanBlock, COND_None);
}

void AD1WeaponBase::Destroyed()
{
	if (AD1Player* Player = Cast<AD1Player>(GetOwner()))
	{
		if (UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent)
		{
			TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
			Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = nullptr;
		}
	}
	
	Super::Destroyed();
}

void AD1WeaponBase::OnRep_EquipmentSlotType()
{
	AD1Player* Player = Cast<AD1Player>(GetOwner());
	UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent;
	TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
	Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = this;
}
