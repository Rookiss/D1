#pragma once

#include "D1Define.h"
#include "D1CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FD1DefaultArmorMeshSet
{
	GENERATED_BODY()
	
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
	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeEnum="EArmorType"))
	TSoftObjectPtr<USkeletalMesh> DefaultMeshEntries[(int32)EArmorType::Count];

	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeEnum="EArmorType"))
	TSoftObjectPtr<USkeletalMesh> SecondaryMeshEntries[(int32)EArmorType::Count];
};

UCLASS(BlueprintType, Const)
class UD1CharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UD1CharacterData& Get();

public:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif // WITH_EDITOR
	
public:
	const FD1DefaultArmorMeshSet& GetDefaultArmorMeshSet(ECharacterSkinType CharacterSkinType) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ECharacterSkinType, FD1DefaultArmorMeshSet> DefaultArmorMeshMap;
};
