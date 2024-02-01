#pragma once

#include "D1Character.h"
#include "D1Player.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UD1EquipmentManagerComponent;
class UD1InventoryManagerComponent;

UCLASS()
class AD1Player : public AD1Character
{
	GENERATED_BODY()
	
public:
	AD1Player(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitAbilitySystem() override;

public:
	float CalculateAimPitch();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponLeftMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponRightMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HelmetMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ChestMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> LegsMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HandsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FootMeshComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float TargetAimPitch = 0.f;
	float CurrentAimPitch = 0.f;
};
