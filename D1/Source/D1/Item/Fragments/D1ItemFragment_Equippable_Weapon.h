#pragma once

#include "D1ItemFragment_Equippable.h"
#include "D1ItemFragment_Equippable_Weapon.generated.h"

UENUM()
enum class EWeaponType
{
	Sword,
	Dagger,
	MagicStuff,
	Shield,

	Count	UMETA(Hidden)
};

UENUM()
enum class EWeaponHandType
{
	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};

UCLASS()
class UD1ItemFragment_Equippable_Weapon : public UD1ItemFragment_Equippable
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	UPROPERTY(EditDefaultsOnly)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;
};
