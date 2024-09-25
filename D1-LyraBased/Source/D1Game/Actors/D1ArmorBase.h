#pragma once

#include "D1ArmorBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AD1ArmorBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ArmorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeActor(TSoftObjectPtr<USkeletalMesh> InDefaultArmorMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryArmorMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial);

public:
	void SetPrimaryArmorMesh(USkeletalMesh* InPrimaryArmorMesh);
	void ShouldUseSecondaryMesh(bool bInShouldUseSecondary);

private:
	void RefreshArmorMesh();
	void SetArmorMesh(TSoftObjectPtr<USkeletalMesh> InArmorMesh);
	
public:
	USkeletalMeshComponent* GetMeshComponent() const { return ArmorMeshComponent; }
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ArmorMeshComponent;

protected:
	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> DefaultArmorMesh;
	
	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> PrimaryArmorMesh;

	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> SecondaryArmorMesh;

protected:
	UPROPERTY()
	FName SkinMaterialSlotName;

	UPROPERTY()
	TSoftObjectPtr<UMaterialInterface> SkinMaterial;
	
private:
	bool bShouldUseSecondaryArmor = false;
};
