#pragma once

#define ITEM_ID_COIN 9999

UENUM()
enum class ESlotState : uint8
{
	Default,
	InValid,
	Valid
};

namespace D1ItemSlotColor
{
	inline FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	inline FLinearColor RedColor     = FLinearColor(0.28f, 0.02f, 0.f, 0.32f);
	inline FLinearColor GreenColor   = FLinearColor(0.02f, 0.28f, 0.f, 0.32f);
	inline FLinearColor BlueColor    = FLinearColor(0.f, 0.02f, 0.28f, 0.32f);
}

UENUM()
enum EEquipmentSlotType
{
	Weapon_Primary_LeftHand,
	Weapon_Primary_RightHand,
	Weapon_Primary_TwoHand,
	
	Weapon_Secondary_LeftHand,
	Weapon_Secondary_RightHand,
	Weapon_Secondary_TwoHand,
	
	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,
	
	EquipmentSlotCount	UMETA(Hidden)
};

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

UENUM()
enum class EWeaponSlotType
{
	Primary,
	Secondary,

	Count	UMETA(Hidden)
};

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

UENUM()
enum class EEquipmentType
{
	Weapon,
	Armor,
	
	Count	UMETA(Hidden)
};

UENUM()
enum class EItemRarity : uint8
{
	Junk,
	Poor,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Unique,

	Count	UMETA(Hidden)
};
