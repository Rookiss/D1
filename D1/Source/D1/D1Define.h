#pragma once

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define LOG_CALLINFO(Verbosity) UE_LOG(LogD1, Verbosity, TEXT("%s"), *CALLINFO)
#define LOG(Format, ...) UE_LOG(LogD1, Display, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_WARNING(Format, ...) UE_LOG(LogD1, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_ERROR(Format, ...) UE_LOG(LogD1, Error, TEXT("%s %s"), *CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_TIME(Time, Format, ...) GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_COLOR(Color ,Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, FString::Printf(Format, ##__VA_ARGS__))

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
enum class EWeaponType : uint8
{
	Sword,
	Shield,
	MagicStuff,

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
	const int32 CoinID = 9999;
	const int32 UnarmedID = 1000;
	const FIntPoint UnitInventorySlotSize = FIntPoint(50.f, 50.f);

	const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByWeaponState = {
		{ EEquipmentSlotType::Unarmed },
		{ EEquipmentSlotType::Primary_LeftHand,   EEquipmentSlotType::Primary_RightHand,   EEquipmentSlotType::Primary_TwoHand   },
		{ EEquipmentSlotType::Secondary_LeftHand, EEquipmentSlotType::Secondary_RightHand, EEquipmentSlotType::Secondary_TwoHand }
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
