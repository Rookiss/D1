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

	UFUNCTION(BlueprintCallable)
	void ChangeVisibilityState(bool bNewShouldHidden);

private:
	UFUNCTION()
	void OnRep_TemplateID();

	UFUNCTION()
	void OnRep_EquipmentSlotType();

	UFUNCTION()
	void OnRep_CanBlock();

	UFUNCTION()
	void OnRep_ShouldHidden();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked);

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

	UPROPERTY(ReplicatedUsing=OnRep_ShouldHidden)
	bool bShouldHidden = false;
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_CanBlock)
	bool bCanBlock = false;

private:
	FLyraAbilitySet_GrantedHandles SkillAbilitySetHandles;
};
