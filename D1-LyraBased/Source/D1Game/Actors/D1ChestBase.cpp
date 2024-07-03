#include "D1ChestBase.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ChestBase)

AD1ChestBase::AD1ChestBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
}

void AD1ChestBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsOpened);
}

FD1InteractionInfo AD1ChestBase::GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const
{
	return bIsOpened ? OpenedInteractionInfo : ClosedInteractionInfo;
}

void AD1ChestBase::OnRep_IsOpened()
{
	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		if (bIsOpened)
		{
			AnimInstance->Montage_Play(CloseMontage);
		}
		else
		{
			AnimInstance->Montage_Play(OpenMontage);
		}
	}
}
