#pragma once

#include "ModularAIController.h"
#include "Teams/D1TeamAgentInterface.h"
#include "D1MonsterAIController.generated.h"

class UAISenseConfig_Sight;

UCLASS(Blueprintable)
class AD1MonsterAIController : public AModularAIController, public ID1TeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AD1MonsterAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	virtual void OnPlayerStateChanged();

	virtual void BeginPlay() override;
	virtual void OnUnPossess() override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	virtual FOnD1TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);
	
private:
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	void BroadcastOnPlayerStateChanged();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|AI Controller")
	float CrowdCollisionQueryRange = 600.f;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfigSight;

private:
	UPROPERTY()
	FOnD1TeamIndexChangedDelegate OnTeamChangedDelegate;
	
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;
};
