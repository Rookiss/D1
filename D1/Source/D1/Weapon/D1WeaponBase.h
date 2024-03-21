#pragma once

#include "D1Define.h"
#include "D1WeaponBase.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS(BlueprintType, Abstract)
class AD1WeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1WeaponBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;

private:
	UFUNCTION()
	void OnRep_ItemID();

	UFUNCTION()
	void OnRep_EquipmentSlotType();
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_ItemID)
	int32 ItemID;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquipmentSlotType)
	EEquipmentSlotType EquipmentSlotType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;
};
