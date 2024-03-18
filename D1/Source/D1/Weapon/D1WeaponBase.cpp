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
	
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMeshComponent->SetupAttachment(GetRootComponent());
	WeaponMeshComponent->SetCollisionProfileName("Weapon");
	WeaponMeshComponent->SetGenerateOverlapEvents(false);
	
	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetupAttachment(GetRootComponent());
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
}

void AD1WeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AD1Player* Player = Cast<AD1Player>(GetOwner());
	UD1EquipManagerComponent* EquipManager = Player->EquipManagerComponent;
	TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
	Entries[(int32)EquipmentSlotType].SpawnedWeaponActor = this;

	WeaponMeshComponent->SetVisibility(false, true);
}

void AD1WeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemID);
	DOREPLIFETIME(ThisClass, EquipmentSlotType);
	DOREPLIFETIME(ThisClass, bCanBlock);
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
