#include "D1ArmorBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ArmorBase)

AD1ArmorBase::AD1ArmorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArmorMeshComponent");
	ArmorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ArmorMeshComponent);
}

void AD1ArmorBase::InitializeActor(USkeletalMesh* InDefaultArmorMesh)
{
	DefaultArmorMesh = InDefaultArmorMesh;
	ArmorMeshComponent->SetSkeletalMesh(DefaultArmorMesh);
}

void AD1ArmorBase::SetArmorMesh(USkeletalMesh* InArmorMesh)
{
	ArmorMesh = InArmorMesh;
	RefreshArmorMesh();
}

void AD1ArmorBase::SetArmorShouldDefault(bool bInShouldDefault)
{
	bShouldArmorDefault = bInShouldDefault;
	RefreshArmorMesh();
}

void AD1ArmorBase::RefreshArmorMesh()
{
	ArmorMeshComponent->SetSkeletalMesh((bShouldArmorDefault || ArmorMesh == nullptr) ? DefaultArmorMesh : ArmorMesh);
}
