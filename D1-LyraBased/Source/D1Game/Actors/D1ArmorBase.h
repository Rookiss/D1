#pragma once

#include "D1ArmorBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AD1ArmorBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ArmorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeActor(USkeletalMesh* InDefaultArmorMesh, USkeletalMesh* InSecondaryArmorMesh);

public:
	void SetPrimaryArmorMesh(USkeletalMesh* InPrimaryArmorMesh);
	void ShouldUseSecondaryMesh(bool bInShouldUseSecondary);

private:
	void RefreshArmorMesh();

public:
	USkeletalMeshComponent* GetMeshComponent() const { return ArmorMeshComponent; }
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ArmorMeshComponent;

protected:
	UPROPERTY()
	TObjectPtr<USkeletalMesh> DefaultArmorMesh;
	
	UPROPERTY()
	TObjectPtr<USkeletalMesh> PrimaryArmorMesh;

	UPROPERTY()
	TObjectPtr<USkeletalMesh> SecondaryArmorMesh;

private:
	bool bShouldUseSecondaryArmor = false;
};
