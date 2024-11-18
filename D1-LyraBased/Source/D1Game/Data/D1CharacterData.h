#pragma once

#include "D1Define.h"
#include "D1CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FD1ArmorMeshEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh = nullptr;
};

USTRUCT(BlueprintType)
struct FD1DefaultArmorMeshSet
{
	GENERATED_BODY()

public:
	FD1DefaultArmorMeshSet();
	
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
	TArray<FD1ArmorMeshEntry> DefaultMeshEntries;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FD1ArmorMeshEntry> SecondaryMeshEntries;
};

UCLASS(BlueprintType, Const)
class UD1CharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UD1CharacterData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
public:
	const FD1DefaultArmorMeshSet& GetDefaultArmorMeshSet(ECharacterSkinType CharacterSkinType) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ECharacterSkinType, FD1DefaultArmorMeshSet> DefaultArmorMeshMap;
};
