#pragma once

#define ITEM_ID_COIN 9999

static inline FIntPoint UnitInventorySlotSize = FIntPoint(50.f, 50.f);

UENUM()
enum class ESlotState : uint8
{
	Default,
	Invalid,
	Valid
};

namespace ItemSlotColor
{
	inline FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	inline FLinearColor RedColor     = FLinearColor(0.28f, 0.02f, 0.f, 0.32f);
	inline FLinearColor GreenColor   = FLinearColor(0.02f, 0.28f, 0.f, 0.32f);
	inline FLinearColor BlueColor    = FLinearColor(0.f, 0.02f, 0.28f, 0.32f);
}

static inline TArray<FLinearColor> ItemRarityColors = {
	FLinearColor::Gray,
	FLinearColor(FColor::Silver),
	FLinearColor::White,
	FLinearColor(FColor::Green),
	FLinearColor(FColor::Cyan),
	FLinearColor(FColor::Purple),
	FLinearColor(FColor::Orange),
	FLinearColor(FColor::Yellow)
};

UENUM()
enum EEquipmentSlotType
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
	Hands,
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
