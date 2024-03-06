#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_ComboAttack.generated.h"

UCLASS()
class UD1GameplayAbility_ComboAttack : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void CheckAttackHit();

	UFUNCTION(BlueprintCallable)
	void CheckBlockHit();
	
	UFUNCTION(BlueprintCallable)
	bool CheckInputPress();
	
protected:
	UPROPERTY(BlueprintReadWrite)
	bool bIsFirstCheck = true;
	
	UPROPERTY(BlueprintReadWrite)
	FVector PrevWeaponLocation;

	UPROPERTY(BlueprintReadWrite)
	FRotator PrevWeaponRotation;

protected:
	UPROPERTY(BlueprintReadWrite)
	TSet<TObjectPtr<AActor>> HitActors;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStage = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bInputPressed = false;

	UPROPERTY(BlueprintReadWrite)
	bool bBlocked = false;
};
