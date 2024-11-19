#pragma once

#include "D1Define.h"
#include "Components/PawnComponent.h"
#include "D1CosmeticManagerComponent.generated.h"

class AD1ArmorBase;
class UD1ItemFragment_Equipable_Armor;


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
	void RefreshArmorMesh(EArmorType ArmorType, const UD1ItemFragment_Equipable_Armor* ArmorFragment);
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

private:
	UPROPERTY()
	TObjectPtr<UChildActorComponent> HeadSlot;
	
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CosmeticSlots;

	bool bInitialized = false;
};
