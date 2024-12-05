#pragma once

#include "AbilitySystemInterface.h"
#include "D1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "D1EquipmentBase.generated.h"

class UAbilitySystemComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class UBoxComponent;

UCLASS(BlueprintType, Abstract)
class AD1EquipmentBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1EquipmentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;

public:
	void Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeBlockState(bool bShouldBlock);

private:
	UFUNCTION()
	void OnRep_EquipmentSlotType();

	UFUNCTION()
	void OnRep_CanBlock();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	int32 GetTemplateID() const { return TemplateID; }
	EEquipmentSlotType GetEquipmentSlotType() const { return EquipmentSlotType; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetEquipMontage();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;
	
protected:
	UPROPERTY(Replicated)
	int32 TemplateID;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquipmentSlotType)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_CanBlock)
	bool bCanBlock = false;

public:
	bool bOnlyUseForLocal = false;
	
private:
	FLyraAbilitySet_GrantedHandles SkillAbilitySetHandles;
};
