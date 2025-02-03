#pragma once

#include "GameFramework/Info.h"
#include "System/GameplayTagStack.h"
#include "D1TeamInfoBase.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UD1TeamCreationComponent;
class UD1TeamSubsystem;

UCLASS(Abstract)
class AD1TeamInfoBase : public AInfo
{
	GENERATED_BODY()

public:
	AD1TeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void RegisterWithTeamSubsystem(UD1TeamSubsystem* Subsystem);
	
protected:
	void TryRegisterWithTeamSubsystem();
	
public:
	int32 GetTeamID() const { return TeamID; }

private:
	void SetTeamID(int32 NewTeamId);

	UFUNCTION()
	void OnRep_TeamID();

public:
	friend UD1TeamCreationComponent;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamID)
	int32 TeamID;
};
