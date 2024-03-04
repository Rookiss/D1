#pragma once

#include "D1GameplayAbility.h"
#include "D1GameplayAbility_ComboAttack.generated.h"

class AD1WeaponBase;

UCLASS()
class UD1GameplayAbility_ComboAttack : public UD1GameplayAbility
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void CheckWeaponOverlap();

	UFUNCTION(BlueprintCallable)
	bool CheckInputPress();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AD1WeaponBase> WeaponActor;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFirstCheck = true;
	
	UPROPERTY(BlueprintReadWrite)
	FVector PrevWeaponLocation;

	UPROPERTY(BlueprintReadWrite)
	FRotator PrevWeaponRotation;

protected:
	UPROPERTY(BlueprintReadWrite)
	TSet<TObjectPtr<AActor>> HittedActors;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStage = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bInputPressed = false;
};
