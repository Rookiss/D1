#pragma once

#include "D1Define.h"
#include "Components/PawnComponent.h"
#include "D1CosmeticManagerComponent.generated.h"

class UD1ItemFragment_Equippable_Armor;
class UD1ItemTemplate;
class AD1ArmorBase;

USTRUCT(BlueprintType)
struct FD1CosmeticDefaultMeshEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;
};

UCLASS(BlueprintType)
class UD1CosmeticManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UD1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	void AddArmorMesh(const UD1ItemFragment_Equippable_Armor* ArmorFragment);
	void RemoveArmorMesh(EArmorType ArmorType);
	void SetArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr);
	
private:
	void InitializeComponent();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Cosmetic")
	TSubclassOf<AD1ArmorBase> CosmeticSlotClass;

	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category="Cosmetic")
	TArray<FD1CosmeticDefaultMeshEntry> DefaultMeshes;

private:
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CosmeticSlots;

	bool bInitialized = false;
	bool bHasFullBodyChest = false;
};
