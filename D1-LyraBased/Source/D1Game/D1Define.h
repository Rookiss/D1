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

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Junk,
	Poor,
	Normal,
	Special,
	Rare,
	Epic,
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

namespace Item
{
	const int32 GoldID = 9999;
	const FIntPoint UnitInventorySlotSize = FIntPoint(45.f, 45.f);

	const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByEquipState = {
		{ EEquipmentSlotType::Unarmed_LeftHand,    EEquipmentSlotType::Unarmed_RightHand                                            },
		{ EEquipmentSlotType::Primary_LeftHand,    EEquipmentSlotType::Primary_RightHand,    EEquipmentSlotType::Primary_TwoHand    },
		{ EEquipmentSlotType::Secondary_LeftHand,  EEquipmentSlotType::Secondary_RightHand,  EEquipmentSlotType::Secondary_TwoHand  },
		{ EEquipmentSlotType::Utility_Primary   },
		{ EEquipmentSlotType::Utility_Secondary },
	};
	
	const FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	const FLinearColor RedColor     = FLinearColor(0.28f, 0.02f, 0.f, 0.32f);
	const FLinearColor GreenColor   = FLinearColor(0.02f, 0.28f, 0.f, 0.32f);
	const FLinearColor BlueColor    = FLinearColor(0.f, 0.02f, 0.28f, 0.32f);

	const TArray<FColor> ItemRarityBackgroundColors = {
		FColor(28, 28, 28),
		FColor(56, 56, 56),
		FColor(18, 31, 18),
		FColor(24, 35, 52),
		FColor(38, 23, 58),
		FColor(59, 40, 23),
		FColor(69, 61, 36),
	};

	const TArray<FColor> ItemRarityTextColors = {
		FColor(130, 130, 130),
		FColor(205, 205, 205),
		FColor(85, 107, 69),
		FColor(88, 115, 138),
		FColor(145, 90, 169),
		FColor(168, 121, 89),
		FColor(196, 175, 104),
	};
}
