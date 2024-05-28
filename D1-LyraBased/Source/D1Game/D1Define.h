#pragma once

#define FORCE_DISABLE_DRAW_DEBUG 0

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

	Tertiary_LeftHand, 
	Tertiary_RightHand, 
	Tertiary_TwoHand,

	Quaternary_LeftHand,
	Quaternary_RightHand, 
	Quaternary_TwoHand,
	
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
	Sword,
	Shield,
	Bow,

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
enum class EWeaponEquipState : uint8
{
	Unarmed,
	Primary,
	Secondary,
	Tertiary,
	Quaternary,

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
enum class EEquipmentType : uint8
{
	Weapon,
	Armor,
	
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
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

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	Projectile,
	AOE,

	Count	UMETA(Hidden)
};

namespace Item
{
	const int32 CoinID = 9999, UnarmedLeftID = 1001, UnarmedRightID = 1002, ShortSwordID = 1003, ShieldID = 1004, BowID = 1005, LongSwordID = 1006;
	const FIntPoint UnitInventorySlotSize = FIntPoint(50.f, 50.f);

	const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByWeaponState = {
		{ EEquipmentSlotType::Unarmed_LeftHand,    EEquipmentSlotType::Unarmed_RightHand                                            },
		{ EEquipmentSlotType::Primary_LeftHand,    EEquipmentSlotType::Primary_RightHand,    EEquipmentSlotType::Primary_TwoHand    },
		{ EEquipmentSlotType::Secondary_LeftHand,  EEquipmentSlotType::Secondary_RightHand,  EEquipmentSlotType::Secondary_TwoHand  },
		{ EEquipmentSlotType::Tertiary_LeftHand,   EEquipmentSlotType::Tertiary_RightHand,   EEquipmentSlotType::Tertiary_TwoHand   },
		{ EEquipmentSlotType::Quaternary_LeftHand, EEquipmentSlotType::Quaternary_RightHand, EEquipmentSlotType::Quaternary_TwoHand }
	};
	
	const FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	const FLinearColor RedColor     = FLinearColor(0.28f, 0.02f, 0.f, 0.32f);
	const FLinearColor GreenColor   = FLinearColor(0.02f, 0.28f, 0.f, 0.32f);
	const FLinearColor BlueColor    = FLinearColor(0.f, 0.02f, 0.28f, 0.32f);

	const TArray<FLinearColor> ItemRarityColors = {
		FLinearColor::Gray,
		FLinearColor(FColor::Silver),
		FLinearColor::White,
		FLinearColor(FColor::Green),
		FLinearColor(FColor::Cyan),
		FLinearColor(FColor::Purple),
		FLinearColor(FColor::Orange),
		FLinearColor(FColor::Yellow)
	};
}
