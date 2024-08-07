#pragma once

#include "D1Define.h"
#include "Components/PawnComponent.h"
#include "D1CosmeticManagerComponent.generated.h"

class AD1ArmorBase;
class UD1ItemFragment_Equippable_Armor;

USTRUCT(BlueprintType)
struct FD1CosmeticMeshEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UD1CosmeticManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UD1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	void RefreshArmorMesh(EArmorType ArmorType, const UD1ItemFragment_Equippable_Armor* ArmorFragment);
	void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const;

private:
	void Initialize();
	UChildActorComponent* SpawnCosmeticSlotActor(USkeletalMesh* DefaultMesh, USkeletalMesh* SecondaryMesh);
	void SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Cosmetic")
	TSubclassOf<AD1ArmorBase> CosmeticSlotClass;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Cosmetic")
	TObjectPtr<USkeletalMesh> HeadDefaultMesh;

	UPROPERTY(EditDefaultsOnly, Category="Cosmetic")
	TObjectPtr<USkeletalMesh> HeadSecondaryMesh;

protected:
	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category="Cosmetic")
	TArray<FD1CosmeticMeshEntry> DefaultMeshes;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category="Cosmetic")
	TArray<FD1CosmeticMeshEntry> SecondaryMeshes;

private:
	UPROPERTY()
	TObjectPtr<UChildActorComponent> HeadSlot;
	
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CosmeticSlots;

	bool bInitialized = false;
};
