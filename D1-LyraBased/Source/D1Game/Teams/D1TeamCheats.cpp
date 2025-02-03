#include "D1TeamCheats.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Teams/D1TeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamCheats)

void UD1TeamCheats::CycleTeam()
{
	if (UD1TeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UD1TeamSubsystem>(GetWorld()))
	{
		APlayerController* PC = GetPlayerController();

		const int32 OldTeamID = TeamSubsystem->FindTeamIDFromObject(PC);
		const TArray<int32> TeamIDs = TeamSubsystem->GetTeamIDs();
		
		if (TeamIDs.Num())
		{
			const int32 IndexOfOldTeam = TeamIDs.Find(OldTeamID);
			const int32 IndexToUse = (IndexOfOldTeam + 1) % TeamIDs.Num();

			const int32 NewTeamID = TeamIDs[IndexToUse];
			TeamSubsystem->ChangeTeamForActor(PC, NewTeamID);
		}

		const int32 ActualNewTeamId = TeamSubsystem->FindTeamIDFromObject(PC);
		UE_LOG(LogConsoleResponse, Log, TEXT("Changed to team %d (from team %d)"), ActualNewTeamId, OldTeamID);
	}
}

void UD1TeamCheats::SetTeam(int32 NewTeamID)
{
	if (UD1TeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UD1TeamSubsystem>(GetWorld()))
	{
		if (TeamSubsystem->DoesTeamExist(NewTeamID))
		{
			APlayerController* PC = GetPlayerController();
			TeamSubsystem->ChangeTeamForActor(PC, NewTeamID);
		}
	}
}

void UD1TeamCheats::LogAllTeams()
{
	if (UD1TeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UD1TeamSubsystem>(GetWorld()))
	{
		const TArray<int32> TeamIDs = TeamSubsystem->GetTeamIDs();

		for (const int32 TeamID : TeamIDs)
		{
			UE_LOG(LogConsoleResponse, Log, TEXT("Team ID %d"), TeamID);
		}
	}
}
