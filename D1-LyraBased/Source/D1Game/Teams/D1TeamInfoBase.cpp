#include "D1TeamInfoBase.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Teams/D1TeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamInfoBase)

AD1TeamInfoBase::AD1TeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamID(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void AD1TeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamID, COND_InitialOnly);
}

void AD1TeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void AD1TeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamID != INDEX_NONE)
	{
		UD1TeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UD1TeamSubsystem>();
		if (TeamSubsystem)
		{
			// EndPlay can happen at weird times where the subsystem has already been destroyed
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AD1TeamInfoBase::RegisterWithTeamSubsystem(UD1TeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void AD1TeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamID != INDEX_NONE)
	{
		UD1TeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UD1TeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void AD1TeamInfoBase::SetTeamID(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamID == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamID = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void AD1TeamInfoBase::OnRep_TeamID()
{
	TryRegisterWithTeamSubsystem();
}
