#pragma once

#include "Abilities/GameplayAbility.h"
#include "D1GameplayAbility.generated.h"

class UInputMappingContext;
class AD1HUD;
class AD1PlayerController;
class AD1Character;
class UD1AbilitySystemComponent;

UENUM(BlueprintType)
enum class ED1AbilityActivationPolicy : uint8
{
	Manual,
	OnGiveOrSpawn,
	InputPressed,
	InputHeld
};

USTRUCT(BlueprintType)
struct FAbilityUIInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> AbilityIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AbilityName;
};

UCLASS(Abstract)
class UD1GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	UFUNCTION(BlueprintCallable, meta=(GameplayTagFilter="GameplayCue"))
	void ExecuteGameplayCueWithActivationPredictionKey(FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, meta=(GameplayTagFilter="GameplayCue"))
	void AddGameplayCueWithActivationPredictionKey(FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameter, bool bRemoveOnAbilityEnd);

	UFUNCTION(BlueprintCallable, meta=(GameplayTagFilter="GameplayCue"))
	void RemoveGameplayCueWithActivationPredictionKey(FGameplayTag GameplayCueTag);
	
public:
	void TryActivateAbilityOnGiveOrSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UFUNCTION(BlueprintCallable)
	void AddAbilityInputMappingContext();

	UFUNCTION(BlueprintCallable)
	void RemoveAbilityInputMappingContext();
	
public:
	UFUNCTION(BlueprintCallable)
	UD1AbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UFUNCTION(BlueprintCallable)
	AD1Character* GetCharacter() const;

	UFUNCTION(BlueprintCallable)
	AD1PlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable)
	AD1HUD* GetHUD() const;

	UFUNCTION(BlueprintCallable)
	bool IsInputPressed() const;
	
	ED1AbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ED1AbilityActivationPolicy ActivationPolicy;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FAbilityUIInfo AbilityUIInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> AbilityIMC;
};
