#include "D1WeaponBase.h"

#include "Character/D1Player.h"
#include "Components/ArrowComponent.h"
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

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(GetRootComponent());
	WeaponMesh->SetCollisionProfileName("NoCollision");
	WeaponMesh->SetGenerateOverlapEvents(false);
	
	TraceCollision = CreateDefaultSubobject<UBoxComponent>("TraceCollision");
	TraceCollision->SetupAttachment(GetRootComponent());
	TraceCollision->SetCollisionProfileName("Weapon");
	TraceCollision->SetGenerateOverlapEvents(true);
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
