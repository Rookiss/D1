#include "D1ArmorBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ArmorBase)

AD1ArmorBase::AD1ArmorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArmorMeshComponent");
	ArmorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ArmorMeshComponent);
}

void AD1ArmorBase::SetArmorMesh(USkeletalMesh* ArmorMesh)
{
	ArmorMeshComponent->SetSkeletalMesh(ArmorMesh);
}
