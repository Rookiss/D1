#pragma once

#include "AbilitySystemInterface.h"
#include "D1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "D1WeaponBase.generated.h"

class UAbilitySystemComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class UBoxComponent;

UCLASS(BlueprintType, Abstract)
class AD1WeaponBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1WeaponBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;

public:
	void Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeSkill(int32 AbilitySetIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeBlockState(bool bShouldBlock);
	
private:
	UFUNCTION()
	void OnRep_TemplateID();

	UFUNCTION()
	void OnRep_EquipmentSlotType();

	UFUNCTION()
	void OnRep_CanBlock();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetHitMontage();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_TemplateID)
	int32 TemplateID;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquipmentSlotType)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

public:
	UPROPERTY(ReplicatedUsing=OnRep_CanBlock)
	bool bCanBlock = false;

private:
	FLyraAbilitySet_GrantedHandles SkillAbilitySetHandles;

	UPROPERTY(Transient)
	UAnimMontage* CachedHitMontage;
};
