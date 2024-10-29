#include "Teams/D1TeamAgentInterface.h"

#include "D1LogChannels.h"
#include "Messages/LyraVerbMessage.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamAgentInterface)

UD1TeamAgentInterface::UD1TeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void ID1TeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<ID1TeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID); 
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(LogD1Team, Verbose, TEXT("[%s] %s assigned team %d"), *GetClientServerContextString(ThisObj), *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}

ETeamAttitude::Type ID1TeamAgentInterface::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const ID1TeamAgentInterface* TeamAgent = Cast<ID1TeamAgentInterface>(OtherPawn->GetController()))
		{
			const int32 MyTeamID = GenericTeamIdToInteger(GetGenericTeamId());
			const int32 OtherTeamID = GenericTeamIdToInteger(TeamAgent->GetGenericTeamId());
			
			if ((MyTeamID != OtherTeamID) || (MyTeamID == 0 && OtherTeamID == 0))
			{
				return ETeamAttitude::Hostile;
			}
			else if (MyTeamID == 1 && OtherTeamID == 1)
			{
				return ETeamAttitude::Friendly;
			}
		}
	}

	return ETeamAttitude::Neutral;
}
