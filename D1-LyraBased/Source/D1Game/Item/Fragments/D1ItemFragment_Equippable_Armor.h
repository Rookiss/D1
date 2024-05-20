#pragma once

#include "D1Define.h"
#include "D1ItemFragment_Equippable.h"
#include "D1ItemFragment_Equippable_Armor.generated.h"

UCLASS()
class UD1ItemFragment_Equippable_Armor : public UD1ItemFragment_Equippable
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;
};
