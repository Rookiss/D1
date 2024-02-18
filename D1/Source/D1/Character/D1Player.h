#pragma once

#include "D1Define.h"
#include "D1Character.h"
#include "D1Player.generated.h"

class AD1WeaponBase;
class UCameraComponent;
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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitAbilitySystem() override;

public:
	float CalculateAimPitch();
	
	void DisableInputAndCollision();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetArmorMesh(EArmorType ArmorType, FSoftObjectPath ArmorMeshPath);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<USkeletalMeshComponent>> ArmorMeshComponents;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<USkeletalMesh>> DefaultArmorMeshes;

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<AD1WeaponBase>> WeaponActors; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1EquipmentManagerComponent> EquipmentManagerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManagerComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float TargetAimPitch = 0.f;
	float CurrentAimPitch = 0.f;
};
