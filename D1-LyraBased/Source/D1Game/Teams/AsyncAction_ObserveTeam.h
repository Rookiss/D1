#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "UObject/ScriptInterface.h"
#include "UObject/WeakInterfacePtr.h"
#include "AsyncAction_ObserveTeam.generated.h"

class ID1TeamAgentInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamObservedAsyncDelegate, bool, bTeamSet, int32, TeamID);

UCLASS()
class UAsyncAction_ObserveTeam : public UCancellableAsyncAction
{
	GENERATED_UCLASS_BODY()

public:
	// 실행하자마자 현재 팀 ID를 알려주고, 그 이후에는 팀이 변경될 때마다 알려준다
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", Keywords="Watch"))
	static UAsyncAction_ObserveTeam* ObserveTeam(UObject* TeamAgent);

protected:
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

public:
	UPROPERTY(BlueprintAssignable)
	FTeamObservedAsyncDelegate OnTeamChanged;

private:
	static UAsyncAction_ObserveTeam* InternalObserveTeamChanges(TScriptInterface<ID1TeamAgentInterface> TeamActor);
	
	UFUNCTION()
	void OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	TWeakInterfacePtr<ID1TeamAgentInterface> TeamInterfacePtr;
};
