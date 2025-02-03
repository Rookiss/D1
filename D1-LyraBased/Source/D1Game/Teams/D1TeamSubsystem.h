#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "D1TeamSubsystem.generated.h"

class ID1TeamAgentInterface;
class AD1TeamPublicInfo;
class AD1TeamPrivateInfo;
class UD1TeamDisplayAsset;
class ALyraPlayerState;
class AD1TeamInfoBase;
class FSubsystemCollectionBase;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnD1TeamDisplayAssetChangedDelegate, const UD1TeamDisplayAsset*, DisplayAsset);

USTRUCT()
struct FD1TeamTrackingInfo
{
	GENERATED_BODY()

public:
	void SetTeamInfo(AD1TeamInfoBase* Info);
	void RemoveTeamInfo(AD1TeamInfoBase* Info);
	
public:
	UPROPERTY()
	TObjectPtr<AD1TeamPublicInfo> PublicInfo = nullptr;

	UPROPERTY()
	TObjectPtr<AD1TeamPrivateInfo> PrivateInfo = nullptr;

	UPROPERTY()
	TObjectPtr<UD1TeamDisplayAsset> DisplayAsset = nullptr;

	UPROPERTY()
	FOnD1TeamDisplayAssetChangedDelegate OnTeamDisplayAssetChanged;
};

// Result of comparing the team affiliation for two actors
UENUM(BlueprintType)
enum class ED1TeamComparison : uint8
{
	// Both actors are members of the same team
	OnSameTeam,

	// The actors are members of opposing teams
	DifferentTeams,

	// One (or both) of the actors was invalid or not part of any team
	InvalidArgument
};


// Team Subsystem의 목적
// - UI 및 각종 Info 공유(월드맵/미니맵/나침반/TeamInfo)
// - 같은 팀원들의 스폰 장소 일치
// - 탈출에 대한 델리게이트 알림
// 데미지는 몬스터끼리만 면역, 나머지 경우는 모두 가능 (팀킬 가능)

UCLASS()
class D1GAME_API UD1TeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UD1TeamSubsystem();

	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	// Tries to registers a new team
	bool RegisterTeamInfo(AD1TeamInfoBase* TeamInfo);

	// Tries to unregister a team, will return false if it didn't work
	bool UnregisterTeamInfo(AD1TeamInfoBase* TeamInfo);

	// Changes the team associated with this actor if possible
	// Note: This function can only be called on the authority
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId);

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	int32 FindTeamIDFromObject(const UObject* TestObject) const;

	const ID1TeamAgentInterface* FindTeamAgentFromObject(const UObject* TestObject) const;

	// Returns the associated player state for this actor, or INDEX_NONE if it is not associated with a player
	const ALyraPlayerState* FindPlayerStateFromActor(const AActor* PossibleTeamActor) const;

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Teams, meta=(Keywords="Get"))
	void FindTeamIDFromActor(const UObject* TestActor, bool& bIsValidTeam, int32& TeamID) const;

	// Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Teams, meta=(ExpandEnumAsExecs=ReturnValue))
	ED1TeamComparison CompareTeams(const UObject* A, const UObject* B, int32& TeamID_A, int32& TeamID_B) const;

	// Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	ED1TeamComparison CompareTeams(const UObject* A, const UObject* B) const;

	// Returns true if the instigator can damage the target, taking into account the friendly fire settings
	bool CanCauseDamage(const AActor* Instigator, const AActor* Target, bool bAllowDamageToSelf = true) const;

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddTeamTagStack(int32 TeamID, FGameplayTag Tag, int32 StackCount, bool bIsPublicInfo = true);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveTeamTagStack(int32 TeamID, FGameplayTag Tag, int32 StackCount, bool bIsPublicInfo = true);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool TeamHasTag(int32 TeamID, FGameplayTag Tag) const;

	// Returns true if the specified team exists
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool DoesTeamExist(int32 TeamID) const;

	// Gets the team display asset for the specified team, from the perspective of the specified team
	// (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	UFUNCTION(BlueprintCallable, Category=Teams)
	UD1TeamDisplayAsset* GetTeamDisplayAsset(int32 TeamID, int32 ViewerTeamID);

	// Gets the team display asset for the specified team, from the perspective of the specified team
	// (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	UFUNCTION(BlueprintCallable, Category = Teams)
	UD1TeamDisplayAsset* GetEffectiveTeamDisplayAsset(int32 TeamID, UObject* ViewerTeamAgent);

	// Gets the list of teams
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Teams)
	TArray<int32> GetTeamIDs() const;

	// Called when a team display asset has been edited, causes all team color observers to update
	void NotifyTeamDisplayAssetModified(UD1TeamDisplayAsset* ModifiedAsset);

	// Register for a team display asset notification for the specified team ID
	FOnD1TeamDisplayAssetChangedDelegate& GetTeamDisplayAssetChangedDelegate(int32 TeamID);

private:
	UPROPERTY()
	TMap<int32, FD1TeamTrackingInfo> TeamMap;

	FDelegateHandle CheatManagerRegistrationHandle;
};
