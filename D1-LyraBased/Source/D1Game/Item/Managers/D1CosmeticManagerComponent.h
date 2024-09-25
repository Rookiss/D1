#pragma once

#include "D1Define.h"
#include "Components/PawnComponent.h"
#include "D1CosmeticManagerComponent.generated.h"

class AD1ArmorBase;
class UD1ItemFragment_Equippable_Armor;

USTRUCT(BlueprintType)
struct FD1CosmeticDefaultMeshEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> DefaultMesh = nullptr;
};

USTRUCT(BlueprintType)
struct FD1CosmeticDefaultMeshSet
{
	GENERATED_BODY()

public:
	FD1CosmeticDefaultMeshSet();
	
public:
	UPROPERTY(VisibleDefaultsOnly)
	ECharacterSkinType CharacterSkinType = ECharacterSkinType::Count;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> UpperBodySkinMaterial;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> LowerBodySkinMaterial;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> HeadDefaultMesh;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> HeadSecondaryMesh;

public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FD1CosmeticDefaultMeshEntry> DefaultMeshEntries;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FD1CosmeticDefaultMeshEntry> SecondaryMeshEntries;
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
	void InitializeManager();
	
	UChildActorComponent* SpawnCosmeticSlotActor(TSoftObjectPtr<USkeletalMesh> InDefaultMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial);
	void SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	ECharacterSkinType CharacterSkinType = ECharacterSkinType::Asian;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AD1ArmorBase> CosmeticSlotClass;

protected:
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FD1CosmeticDefaultMeshSet> CosmeticDefaultMeshSets;
	
private:
	UPROPERTY()
	TObjectPtr<UChildActorComponent> HeadSlot;
	
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CosmeticSlots;

	bool bInitialized = false;
};
