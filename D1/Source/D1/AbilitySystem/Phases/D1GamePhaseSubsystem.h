#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "D1GamePhaseSubsystem.generated.h"

template <typename T> class TSubclassOf;

class UD1GamePhaseAbility;
class UObject;
struct FFrame;
struct FGameplayAbilitySpecHandle;

DECLARE_DYNAMIC_DELEGATE_OneParam(FD1GamePhaseDynamicDelegate, const UD1GamePhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(FD1GamePhaseDelegate, const UD1GamePhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FD1GamePhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FD1GamePhaseTagDelegate, const FGameplayTag& PhaseTag);

UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
	ExactMatch,
	PartialMatch
};

UCLASS()
class UD1GamePhaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UD1GamePhaseSubsystem();

protected:
	virtual void PostInitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	void StartPhase(TSubclassOf<UD1GamePhaseAbility> PhaseAbility, FD1GamePhaseDelegate PhaseEndedCallback = FD1GamePhaseDelegate());
	
	void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FD1GamePhaseTagDelegate& WhenPhaseActive);
	void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FD1GamePhaseTagDelegate& WhenPhaseEnd);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
	bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName="Start Phase", AutoCreateRefTerm = "PhaseEnded"))
	void K2_StartPhase(TSubclassOf<UD1GamePhaseAbility> Phase, const FD1GamePhaseDynamicDelegate& PhaseEnded);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
	void K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FD1GamePhaseTagDynamicDelegate WhenPhaseActive);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
	void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FD1GamePhaseTagDynamicDelegate WhenPhaseEnd);

	void OnBeginPhase(const UD1GamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
	void OnEndPhase(const UD1GamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
	struct FD1GamePhaseEntry
	{
	public:
		FGameplayTag PhaseTag;
		FD1GamePhaseDelegate PhaseEndedCallback;
	};

	TMap<FGameplayAbilitySpecHandle, FD1GamePhaseEntry> ActivePhaseMap;

	struct FPhaseObserver
	{
	public:
		bool IsMatch(const FGameplayTag& ComparePhaseTag) const;
	
		FGameplayTag PhaseTag;
		EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;
		FD1GamePhaseTagDelegate PhaseCallback;
	};

	TArray<FPhaseObserver> PhaseStartObservers;
	TArray<FPhaseObserver> PhaseEndObservers;

	friend class UD1GamePhaseAbility;
};
