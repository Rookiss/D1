#pragma once

#define ITEM_ID_COIN 9999

UENUM(BlueprintType)
enum class ESlotState : uint8
{
	Default,
	Invalid,
	Valid
};

UENUM(BlueprintType)
enum class EEquipSlotType : uint8
{
	Unarmed,
	
	Primary_LeftHand,
	Primary_RightHand,
	Primary_TwoHand,
	
	Secondary_LeftHand,
	Secondary_RightHand,
	Secondary_TwoHand,
	
	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,
	
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Primary_LeftHand,
	Primary_RightHand,
	Primary_TwoHand,
	
	Secondary_LeftHand,
	Secondary_RightHand,
	Secondary_TwoHand,
	
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
	Sword,
	Dagger,
	MagicStuff,
	Shield,

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

namespace Item
{
	const FIntPoint UnitInventorySlotSize = FIntPoint(50.f, 50.f);

	const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByWeaponState = {
		{ },
		{ EEquipmentSlotType::Primary_LeftHand,   EEquipmentSlotType::Primary_RightHand,   EEquipmentSlotType::Primary_TwoHand   },
		{ EEquipmentSlotType::Secondary_LeftHand, EEquipmentSlotType::Secondary_RightHand, EEquipmentSlotType::Secondary_TwoHand }
	};

	const TArray<TArray<EEquipSlotType>> EquipSlotsByWeaponState = {
		{ EEquipSlotType::Unarmed },
		{ EEquipSlotType::Primary_LeftHand,   EEquipSlotType::Primary_RightHand,   EEquipSlotType::Primary_TwoHand   },
		{ EEquipSlotType::Secondary_LeftHand, EEquipSlotType::Secondary_RightHand, EEquipSlotType::Secondary_TwoHand }
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
