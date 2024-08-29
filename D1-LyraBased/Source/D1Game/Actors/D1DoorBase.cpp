#include "D1DoorBase.h"

#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DoorBase)

AD1DoorBase::AD1DoorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	LeftMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftMeshComponent"));
	LeftMeshComponent->SetupAttachment(GetRootComponent());
	LeftMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	LeftMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	LeftMeshComponent->SetCanEverAffectNavigation(true);

	RightMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightMeshComponent"));
	RightMeshComponent->SetupAttachment(GetRootComponent());
	RightMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	RightMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	RightMeshComponent->SetCanEverAffectNavigation(true);
}

void AD1DoorBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DoorState);
}

FD1InteractionInfo AD1DoorBase::GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const
{
	switch (DoorState)
	{
	case EDoorState::Open_Forward:
	case EDoorState::Open_Backward:
		return OpenedInteractionInfo;
		
	case EDoorState::Close:
		return ClosedInteractionInfo;
		
	default:
		return FD1InteractionInfo();
	}
}

void AD1DoorBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	if (LeftMeshComponent->GetStaticMesh())
	{
		OutMeshComponents.Add(LeftMeshComponent);
	}

	if (RightMeshComponent->GetStaticMesh())
	{
		OutMeshComponents.Add(RightMeshComponent);
	}
}

void AD1DoorBase::SetDoorState(EDoorState NewDoorState)
{
	if (HasAuthority() == false || NewDoorState == DoorState)
		return;

	DoorState = NewDoorState;
	OnRep_DoorState();
}

void AD1DoorBase::OnRep_DoorState()
{
	OnDoorStateChanged(DoorState);
}
