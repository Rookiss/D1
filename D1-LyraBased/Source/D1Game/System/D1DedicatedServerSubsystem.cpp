#include "D1DedicatedServerSubsystem.h"

#include "D1LogChannels.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DedicatedServerSubsystem)

UD1DedicatedServerSubsystem::UD1DedicatedServerSubsystem()
{
	
}

void UD1DedicatedServerSubsystem::CreateDedicatedServerSession(FString InMapPath)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
		if (OnlineSessionPtr)
		{
			MapPath = InMapPath;
			
			FOnlineSessionSettings SessionCreationInfo;
			SessionCreationInfo.bIsDedicated = true;
			SessionCreationInfo.bShouldAdvertise = true;
			SessionCreationInfo.NumPublicConnections = 15;
			SessionCreationInfo.bAllowJoinInProgress = true;
			SessionCreationInfo.bUseLobbiesIfAvailable = false;
			SessionCreationInfo.bAllowInvites = false;
			SessionCreationInfo.bUsesPresence = false;
			SessionCreationInfo.bUseLobbiesVoiceChatIfAvailable = false;
			SessionCreationInfo.bAllowJoinViaPresence = false;
			SessionCreationInfo.bAllowJoinViaPresenceFriendsOnly = false;
			SessionCreationInfo.bIsLANMatch = false;

			SessionCreationInfo.Set(SEARCH_KEYWORDS, FString("D1TestRandom"), EOnlineDataAdvertisementType::ViaOnlineService);
			OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnSessionCreationReply);
			OnlineSessionPtr->CreateSession(0, FName("D1TestRandom"), SessionCreationInfo);
		}
	}
}

void UD1DedicatedServerSubsystem::OnSessionCreationReply(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogD1, Warning, TEXT("Session reply : "));
	if (bWasSuccessful)
	{
		UE_LOG(LogD1, Warning, TEXT("Session Creation Successful"));
		GetWorld()->ServerTravel(MapPath);
	}
	else
	{
		UE_LOG(LogD1, Error, TEXT("Session Creation Failed"));
	}
}
