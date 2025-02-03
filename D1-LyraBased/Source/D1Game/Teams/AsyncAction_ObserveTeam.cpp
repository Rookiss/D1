#include "AsyncAction_ObserveTeam.h"

#include "D1TeamAgentInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ObserveTeam)

UAsyncAction_ObserveTeam::UAsyncAction_ObserveTeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UAsyncAction_ObserveTeam* UAsyncAction_ObserveTeam::ObserveTeam(UObject* TeamAgent)
{
	return InternalObserveTeamChanges(TeamAgent);
}

UAsyncAction_ObserveTeam* UAsyncAction_ObserveTeam::InternalObserveTeamChanges(TScriptInterface<ID1TeamAgentInterface> TeamActor)
{
	UAsyncAction_ObserveTeam* Action = nullptr;

	if (TeamActor)
	{
		Action = NewObject<UAsyncAction_ObserveTeam>();
		Action->TeamInterfacePtr = TeamActor;
		Action->RegisterWithGameInstance(TeamActor.GetObject());
	}

	return Action;
}

void UAsyncAction_ObserveTeam::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();
	
	if (ID1TeamAgentInterface* TeamInterface = TeamInterfacePtr.Get())
	{
		TeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
	}
}

void UAsyncAction_ObserveTeam::Activate()
{
	bool bCouldSucceed = false;
	int32 CurrentTeamIndex = INDEX_NONE;

	if (ID1TeamAgentInterface* TeamInterface = TeamInterfacePtr.Get())
	{
		CurrentTeamIndex = GenericTeamIDToInteger(TeamInterface->GetGenericTeamId());
		TeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnWatchedAgentChangedTeam);
		bCouldSucceed = true;
	}
	
	OnTeamChanged.Broadcast(CurrentTeamIndex != INDEX_NONE, CurrentTeamIndex);
	
	if (bCouldSucceed == false)
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_ObserveTeam::OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	OnTeamChanged.Broadcast(NewTeam != INDEX_NONE, NewTeam);
}
