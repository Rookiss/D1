#pragma once

#include "D1ItemFragment_Equippable.h"
#include "D1ItemFragment_Equippable_Armor.generated.h"

UENUM()
enum class EArmorType
{
	Helmet,
	Chest,
	Legs,
	Hand,
	Foot,

	Count	UMETA(Hidden)
};

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
