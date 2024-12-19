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

	UFUNCTION(BlueprintCallable)
	void ProcessEquip();
	
private:
	void CheckPropertyInitialization();
	
	UFUNCTION()
	void OnRep_ItemTemplateID();
	
	UFUNCTION()
	void OnRep_EquipmentSlotType();

	UFUNCTION()
	void OnRep_CanBlock();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	int32 GetItemTemplateID() const { return ItemTemplateID; }
	EEquipmentSlotType GetEquipmentSlotType() const { return EquipmentSlotType; }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TSoftObjectPtr<UAnimMontage> GetEquipMontage();
	
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
	UPROPERTY(ReplicatedUsing=OnRep_ItemTemplateID)
	int32 ItemTemplateID = INDEX_NONE;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquipmentSlotType)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_CanBlock)
	bool bCanBlock = false;

public:
	bool bOnlyUseForLocal = false;
	bool bInitialized = false;
	
private:
	FLyraAbilitySet_GrantedHandles SkillAbilitySetHandles;
};
