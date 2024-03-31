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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;

private:
	UFUNCTION()
	void OnRep_TemplateID();

	UFUNCTION()
	void OnRep_EquipmentSlotType();
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_TemplateID)
	int32 TemplateID;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquipmentSlotType)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;

public:
	UPROPERTY(Replicated)
	bool bCanBlock = false;
};
