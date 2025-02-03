#pragma once

#include "GameFramework/CheatManager.h"
#include "D1TeamCheats.generated.h"

UCLASS()
class UD1TeamCheats : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	virtual void CycleTeam();
	
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	virtual void SetTeam(int32 NewTeamID);
	
	UFUNCTION(Exec)
	virtual void LogAllTeams();
};
