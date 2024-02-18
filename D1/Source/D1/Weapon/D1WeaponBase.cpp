#include "D1WeaponBase.h"

#include "Components/BoxComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WeaponBase)

AD1WeaponBase::AD1WeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionProfileName("NoCollision");
	
	HitCollision = CreateDefaultSubobject<UBoxComponent>("HitCollision");
	HitCollision->SetupAttachment(WeaponMesh);
	HitCollision->SetCollisionProfileName("OverlapAllDynamic");
}
