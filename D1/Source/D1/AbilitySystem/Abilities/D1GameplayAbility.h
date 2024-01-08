#pragma once

#include "Abilities/GameplayAbility.h"
#include "D1GameplayAbility.generated.h"

class AD1Character;
class UD1AbilitySystemComponent;

UENUM(BlueprintType)
enum class ED1AbilityActivationPolicy : uint8
{
	Manual,
	OnSpawn,
	InputPressed,
	InputHeld
};

UCLASS(Abstract)
class UD1GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	
public:
	UFUNCTION(BlueprintCallable)
	UD1AbilitySystemComponent* GetD1AbilitySystemComponent() const;
	
	UFUNCTION(BlueprintCallable)
	AD1Character* GetD1Character() const;

	ED1AbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ED1AbilityActivationPolicy ActivationPolicy;
};
