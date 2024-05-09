#pragma once

#include "D1Character.h"
#include "D1Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UD1InventoryManagerComponent;
class UD1EquipmentManagerComponent;
class UD1EquipManagerComponent;

UCLASS()
class AD1Player : public AD1Character
{
	GENERATED_BODY()
	
public:
	AD1Player(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	virtual void InitAbilitySystem() override;
	virtual void StartDeath() override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	float CalculateAimPitch();

public:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<USkeletalMeshComponent>> ArmorMeshComponents;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<USkeletalMesh>> DefaultArmorMeshes;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1EquipManagerComponent> EquipManagerComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float TargetAimPitch = 0.f;
	float CurrentAimPitch = 0.f;

	float CurrentCameraHeight = 80.f;
	float TargetCameraHeight = 80.f;
	float DefaultCameraHeight = 80.f;
};
