#include "Teams/D1TeamSubsystem.h"

#include "AbilitySystemGlobals.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "D1LogChannels.h"
#include "D1TeamPrivateInfo.h"
#include "D1TeamPublicInfo.h"
#include "D1TeamAgentInterface.h"
#include "D1TeamCheats.h"
#include "Player/LyraPlayerState.h"
#include "D1TeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1TeamSubsystem)

void FD1TeamTrackingInfo::SetTeamInfo(AD1TeamInfoBase* Info)
{
	if (AD1TeamPublicInfo* NewPublicInfo = Cast<AD1TeamPublicInfo>(Info))
	{
		ensure((PublicInfo == nullptr) || (PublicInfo == NewPublicInfo));
		PublicInfo = NewPublicInfo;

		UD1TeamDisplayAsset* OldDisplayAsset = DisplayAsset;
		DisplayAsset = NewPublicInfo->GetTeamDisplayAsset();

		if (OldDisplayAsset != DisplayAsset)
		{
			OnTeamDisplayAssetChanged.Broadcast(DisplayAsset);
		}
	}
	else if (AD1TeamPrivateInfo* NewPrivateInfo = Cast<AD1TeamPrivateInfo>(Info))
	{
		ensure((PrivateInfo == nullptr) || (PrivateInfo == NewPrivateInfo));
		PrivateInfo = NewPrivateInfo;
	}
	else
	{
		checkf(false, TEXT("Expected a public or private team info but got %s"), *GetPathNameSafe(Info))
	}
}

void FD1TeamTrackingInfo::RemoveTeamInfo(AD1TeamInfoBase* Info)
{
	if (PublicInfo == Info)
	{
		PublicInfo = nullptr;
	}
	else if (PrivateInfo == Info)
	{
		PrivateInfo = nullptr;
	}
	else
	{
		ensureMsgf(false, TEXT("Expected a previously registered team info but got %s"), *GetPathNameSafe(Info));
	}
}

UD1TeamSubsystem::UD1TeamSubsystem()
{
	
}

void UD1TeamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	auto AddTeamCheats = [](UCheatManager* CheatManager)
	{
		CheatManager->AddCheatManagerExtension(NewObject<UD1TeamCheats>(CheatManager));
	};

	CheatManagerRegistrationHandle = UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(AddTeamCheats));
}

void UD1TeamSubsystem::Deinitialize()
{
	UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerRegistrationHandle);

	Super::Deinitialize();
}

bool UD1TeamSubsystem::RegisterTeamInfo(AD1TeamInfoBase* TeamInfo)
{
	if (ensure(TeamInfo) == false)
		return false;

	const int32 TeamID = TeamInfo->GetTeamID();
	if (ensure(TeamID != INDEX_NONE))
	{
		FD1TeamTrackingInfo& Entry = TeamMap.FindOrAdd(TeamID);
		Entry.SetTeamInfo(TeamInfo);
		return true;
	}

	return false;
}

bool UD1TeamSubsystem::UnregisterTeamInfo(AD1TeamInfoBase* TeamInfo)
{
	if (ensure(TeamInfo) == false)
		return false;

	const int32 TeamID = TeamInfo->GetTeamID();
	if (ensure(TeamID != INDEX_NONE))
	{
		if (FD1TeamTrackingInfo* Entry = TeamMap.Find(TeamID))
		{
			Entry->RemoveTeamInfo(TeamInfo);
			return true;
		}
	}

	return false;
}

bool UD1TeamSubsystem::ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamID)
{
	const FGenericTeamId NewGenericTeamID = IntegerToGenericTeamID(NewTeamID);
	if (ALyraPlayerState* LyraPS = const_cast<ALyraPlayerState*>(FindPlayerStateFromActor(ActorToChange)))
	{
		LyraPS->SetGenericTeamId(NewGenericTeamID);
		return true;
	}
	else if (ID1TeamAgentInterface* TeamActor = Cast<ID1TeamAgentInterface>(ActorToChange))
	{
		TeamActor->SetGenericTeamId(NewGenericTeamID);
		return true;
	}
	else
	{
		return false;
	}
}

int32 UD1TeamSubsystem::FindTeamIDFromObject(const UObject* TestObject) const
{
	if (const ID1TeamAgentInterface* TeamAgentInterface = FindTeamAgentFromObject(TestObject))
	{
		return GenericTeamIDToInteger(TeamAgentInterface->GetGenericTeamId());
	}
	else if (const AD1TeamInfoBase* TeamInfo = Cast<AD1TeamInfoBase>(TestObject))
	{
		return TeamInfo->GetTeamID();
	}
	
	return INDEX_NONE;
}

const ID1TeamAgentInterface* UD1TeamSubsystem::FindTeamAgentFromObject(const UObject* TestObject) const
{
	if (const ID1TeamAgentInterface* ObjectWithTeamInterface = Cast<ID1TeamAgentInterface>(TestObject))
	{
		return ObjectWithTeamInterface;
	}

	if (const AActor* TestActor = Cast<const AActor>(TestObject))
	{
		if (const ID1TeamAgentInterface* InstigatorWithTeamInterface = Cast<ID1TeamAgentInterface>(TestActor->GetInstigator()))
		{
			return InstigatorWithTeamInterface;
		}

		if (const ID1TeamAgentInterface* OwnerWithTeamInterface = Cast<ID1TeamAgentInterface>(TestActor->GetOwner()))
		{
			return OwnerWithTeamInterface;
		}

		if (const ALyraPlayerState* LyraPS = FindPlayerStateFromActor(TestActor))
		{
			return LyraPS;
		}
	}
	
	return nullptr;
}

const ALyraPlayerState* UD1TeamSubsystem::FindPlayerStateFromActor(const AActor* PossibleTeamActor) const
{
	if (PossibleTeamActor)
	{
		if (const APawn* Pawn = Cast<const APawn>(PossibleTeamActor))
		{
			if (ALyraPlayerState* LyraPS = Pawn->GetPlayerState<ALyraPlayerState>())
			{
				return LyraPS;
			}
		}
		else if (const AController* Controller = Cast<const AController>(PossibleTeamActor))
		{
			if (ALyraPlayerState* LyraPS = Cast<ALyraPlayerState>(Controller->PlayerState))
			{
				return LyraPS;
			}
		}
		else if (const ALyraPlayerState* LyraPS = Cast<const ALyraPlayerState>(PossibleTeamActor))
		{
			return LyraPS; 
		}
	}

	return nullptr;
}

ED1TeamComparison UD1TeamSubsystem::CompareTeams(const UObject* A, const UObject* B, int32& TeamID_A, int32& TeamID_B) const
{
	TeamID_A = FindTeamIDFromObject(Cast<const AActor>(A));
	TeamID_B = FindTeamIDFromObject(Cast<const AActor>(B));

	if ((TeamID_A == INDEX_NONE) || (TeamID_B == INDEX_NONE))
	{
		return ED1TeamComparison::InvalidArgument;
	}
	else
	{
		return (TeamID_A == TeamID_B) ? ED1TeamComparison::OnSameTeam : ED1TeamComparison::DifferentTeams; 
	}
}

ED1TeamComparison UD1TeamSubsystem::CompareTeams(const UObject* A, const UObject* B) const
{
	int32 TeamID_A;
	int32 TeamID_B;
	return CompareTeams(A, B, /*out*/ TeamID_A, /*out*/ TeamID_B);
}

void UD1TeamSubsystem::FindTeamIDFromActor(const UObject* TestObject, bool& bIsValidTeam, int32& TeamID) const
{
	TeamID = FindTeamIDFromObject(TestObject);
	bIsValidTeam = DoesTeamExist(TeamID);
}

void UD1TeamSubsystem::AddTeamTagStack(int32 TeamID, FGameplayTag Tag, int32 StackCount, bool bIsPublicInfo)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(LogD1Team, Error, TEXT("AddTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamID, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (FD1TeamTrackingInfo* Entry = TeamMap.Find(TeamID))
	{
		AD1TeamInfoBase* Info;
		if (bIsPublicInfo)
		{
			Info = Entry->PublicInfo;
		}
		else
		{
			Info = Entry->PrivateInfo;
		}
		
		if (Info)
		{
			if (Info->HasAuthority())
			{
				Info->TeamTags.AddStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

void UD1TeamSubsystem::RemoveTeamTagStack(int32 TeamID, FGameplayTag Tag, int32 StackCount, bool bIsPublicInfo)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(LogD1Team, Error, TEXT("RemoveTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamID, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (FD1TeamTrackingInfo* Entry = TeamMap.Find(TeamID))
	{
		AD1TeamInfoBase* Info;
		if (bIsPublicInfo)
		{
			Info = Entry->PublicInfo;
		}
		else
		{
			Info = Entry->PrivateInfo;
		}
		
		if (Info)
		{
			if (Info->HasAuthority())
			{
				Info->TeamTags.RemoveStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

int32 UD1TeamSubsystem::GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const
{
	if (const FD1TeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		const int32 PublicStackCount = Entry->PublicInfo ? Entry->PublicInfo->TeamTags.GetStackCount(Tag) : 0;
		const int32 PrivateStackCount = Entry->PrivateInfo ? Entry->PrivateInfo->TeamTags.GetStackCount(Tag) : 0;
		return PublicStackCount + PrivateStackCount;
	}
	else
	{
		UE_LOG(LogD1Team, Verbose, TEXT("GetTeamTagStackCount(TeamId: %d, Tag: %s) failed because it was passed an unknown team id"), TeamId, *Tag.ToString());
		return 0;
	}
}

bool UD1TeamSubsystem::TeamHasTag(int32 TeamID, FGameplayTag Tag) const
{
	return GetTeamTagStackCount(TeamID, Tag) > 0;
}

bool UD1TeamSubsystem::DoesTeamExist(int32 TeamID) const
{
	return TeamMap.Contains(TeamID);
}

TArray<int32> UD1TeamSubsystem::GetTeamIDs() const
{
	TArray<int32> Result;
	TeamMap.GenerateKeyArray(Result);
	Result.Sort();
	return Result;
}

bool UD1TeamSubsystem::CanCauseDamage(const AActor* Instigator, const AActor* Target, bool bAllowDamageToSelf) const
{
	if (Instigator == nullptr || Target == nullptr)
		return false;
	
	if (bAllowDamageToSelf)
	{
		if ((Instigator == Target) || (FindPlayerStateFromActor(Instigator) == FindPlayerStateFromActor(Target)))
			return true;
	}
	
	const ID1TeamAgentInterface* InstigatorTeamAgent = FindTeamAgentFromObject(Instigator);
	const ID1TeamAgentInterface* TargetTeamAgent = FindTeamAgentFromObject(Target);
	if (InstigatorTeamAgent == nullptr || TargetTeamAgent == nullptr)
	{
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target) != nullptr;
	}
	
	ETeamAttitude::Type TeamAttitudeType = InstigatorTeamAgent->GetTeamAttitudeTowards(*Target);
	return (TeamAttitudeType == ETeamAttitude::Hostile);
}

UD1TeamDisplayAsset* UD1TeamSubsystem::GetTeamDisplayAsset(int32 TeamID, int32 ViewerTeamID)
{
	if (FD1TeamTrackingInfo* Entry = TeamMap.Find(TeamID))
	{
		return Entry->DisplayAsset;
	}

	return nullptr;
}

UD1TeamDisplayAsset* UD1TeamSubsystem::GetEffectiveTeamDisplayAsset(int32 TeamID, UObject* ViewerTeamAgent)
{
	return GetTeamDisplayAsset(TeamID, FindTeamIDFromObject(ViewerTeamAgent));
}

void UD1TeamSubsystem::NotifyTeamDisplayAssetModified(UD1TeamDisplayAsset* /*ModifiedAsset*/)
{
	// Broadcasting to all observers when a display asset is edited right now, instead of only the edited one
	for (const auto& KVP : TeamMap)
	{
		const FD1TeamTrackingInfo& TrackingInfo = KVP.Value;
		TrackingInfo.OnTeamDisplayAssetChanged.Broadcast(TrackingInfo.DisplayAsset);
	}
}

FOnD1TeamDisplayAssetChangedDelegate& UD1TeamSubsystem::GetTeamDisplayAssetChangedDelegate(int32 TeamID)
{
	return TeamMap.FindOrAdd(TeamID).OnTeamDisplayAssetChanged;
}
