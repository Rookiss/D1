#include "D1ArmorBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ArmorBase)

AD1ArmorBase::AD1ArmorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArmorMeshComponent");
	ArmorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ArmorMeshComponent);
}

void AD1ArmorBase::InitializeActor(USkeletalMesh* InDefaultArmorMesh, USkeletalMesh* InSecondaryArmorMesh)
{
	DefaultArmorMesh = InDefaultArmorMesh;
	SecondaryArmorMesh = (InSecondaryArmorMesh ? InSecondaryArmorMesh : InDefaultArmorMesh);
	
	ArmorMeshComponent->SetSkeletalMesh(DefaultArmorMesh);
}

void AD1ArmorBase::SetPrimaryArmorMesh(USkeletalMesh* InPrimaryArmorMesh)
{
	PrimaryArmorMesh = InPrimaryArmorMesh;
	RefreshArmorMesh();
}

void AD1ArmorBase::ShouldUseSecondaryMesh(bool bInShouldUseSecondary)
{
	bShouldUseSecondaryArmor = bInShouldUseSecondary;
	RefreshArmorMesh();
}

void AD1ArmorBase::RefreshArmorMesh()
{
	if (bShouldUseSecondaryArmor)
	{
		ArmorMeshComponent->SetSkeletalMesh(SecondaryArmorMesh);
	}
	else
	{
		ArmorMeshComponent->SetSkeletalMesh(PrimaryArmorMesh ? PrimaryArmorMesh : DefaultArmorMesh);
	}
}
