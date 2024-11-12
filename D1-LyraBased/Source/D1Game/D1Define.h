#pragma once

UENUM(BlueprintType)
enum class ESlotState : uint8
{
	Default,
	Invalid,
	Valid
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Unarmed_LeftHand,
	Unarmed_RightHand,
	
	Primary_LeftHand,
	Primary_RightHand,
	Primary_TwoHand,
	
	Secondary_LeftHand,
	Secondary_RightHand,
	Secondary_TwoHand,

	Utility_Primary,
	Utility_Secondary,
	Utility_Tertiary,
	Utility_Quaternary,
	
	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,
	
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	Unarmed,
	
	Weapon_Primary,
	Weapon_Secondary,

	Utility_Primary,
	Utility_Secondary,
	Utility_Tertiary,
	Utility_Quaternary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Armor,
	Weapon,
	Utility,
	
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	OneHandSword,
	TwoHandSword,
	GreatSword,
	Shield,
	Staff,
	Bow,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponSlotType : uint8
{
	Primary,
	Secondary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponHandType : uint8
{
	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EUtilityType : uint8
{
	Drink,
	LightSource,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EUtilitySlotType : uint8
{
	Primary,
	Secondary,
	Tertiary,
	Quaternary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Poor,
	Common,
	Uncommon,
	Rare,
	Legendary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	None,
	Projectile,
	AOE,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EOverlayTargetType : uint8
{
	None,
	Weapon,
	Character,
	All,
};

UENUM(BlueprintType)
enum class ECharacterSkinType : uint8
{
	Asian,
	Black,
	
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ED1TeamID : uint8
{
	NoTeam,
	Monster = 1,
};

namespace Item
{
	const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByEquipState = {
		{ EEquipmentSlotType::Unarmed_LeftHand,    EEquipmentSlotType::Unarmed_RightHand                                            },
		{ EEquipmentSlotType::Primary_LeftHand,    EEquipmentSlotType::Primary_RightHand,    EEquipmentSlotType::Primary_TwoHand    },
		{ EEquipmentSlotType::Secondary_LeftHand,  EEquipmentSlotType::Secondary_RightHand,  EEquipmentSlotType::Secondary_TwoHand  },
		{ EEquipmentSlotType::Utility_Primary }, { EEquipmentSlotType::Utility_Secondary }, { EEquipmentSlotType::Utility_Tertiary }, { EEquipmentSlotType::Utility_Quaternary },
	};
}
