#include "D1TeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "Teams/D1TeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamPublicInfo)

AD1TeamPublicInfo::AD1TeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AD1TeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void AD1TeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<UD1TeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void AD1TeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}
