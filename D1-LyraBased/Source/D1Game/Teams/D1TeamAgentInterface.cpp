#include "D1TeamAgentInterface.h"

#include "D1LogChannels.h"
#include "D1TeamSubsystem.h"
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
		const int32 OldTeamIndex = GenericTeamIDToInteger(OldTeamID); 
		const int32 NewTeamIndex = GenericTeamIDToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(LogD1Team, Verbose, TEXT("[%s] %s assigned team %d"), *GetClientServerContextString(ThisObj), *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}

ETeamAttitude::Type ID1TeamAgentInterface::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (UD1TeamSubsystem* TeamSubsystem = Other.GetWorld()->GetSubsystem<UD1TeamSubsystem>())
	{
		if (const ID1TeamAgentInterface* OtherTeamAgent = TeamSubsystem->FindTeamAgentFromObject(&Other))
		{
			const int32 MyTeamID = GenericTeamIDToInteger(GetGenericTeamId());
			const int32 OtherTeamID = GenericTeamIDToInteger(OtherTeamAgent->GetGenericTeamId());
			const int32 MonsterTeamID = EnumToGenericTeamID(ED1TeamID::Monster);
			
			if (MyTeamID == MonsterTeamID && OtherTeamID == MonsterTeamID)
			{
				return ETeamAttitude::Friendly;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}
	}

	return ETeamAttitude::Neutral;
}
