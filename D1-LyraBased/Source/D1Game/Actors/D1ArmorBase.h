#pragma once

#include "D1ArmorBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AD1ArmorBase : public AActor
{
	GENERATED_BODY()
	
public:
	AD1ArmorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeActor(USkeletalMesh* InDefaultArmorMesh);

public:
	void SetArmorMesh(USkeletalMesh* InArmorMesh);
	void SetArmorShouldDefault(bool bInShouldDefault);

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
	TObjectPtr<USkeletalMesh> ArmorMesh;

private:
	bool bShouldArmorDefault = false;
};
