#include "D1CosmeticWeapon.h"

#include "Components/ArrowComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CosmeticWeapon)

AD1CosmeticWeapon::AD1CosmeticWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMeshComponent->SetCollisionProfileName("NoCollision");
	WeaponMeshComponent->SetGenerateOverlapEvents(false);
	WeaponMeshComponent->SetupAttachment(GetRootComponent());
}
