#pragma once

#include "D1ItemFragment.h"
#include "Item/D1ItemInstance.h"
#include "D1ItemFragment_Equippable.generated.h"

class UD1AbilitySystemData;
class UD1EquipmentInstance;

UENUM()
enum class EEquipmentType : uint8
{
	Weapon,
	Head,
	Chest,
	Legs,
	Hand,
	Foot,
	
	Count	UMETA(Hidden)
};

USTRUCT()
struct FD1EquipmentAttachInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> AttachActorClass;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform;
};

USTRUCT()
struct FStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxValue = 0;
};

USTRUCT()
struct FStatRangeSet
{
	GENERATED_BODY()

public:
	FStatRangeSet();
	
public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize, meta=(ForceInlineRow))
	TMap<EItemRarity, FStatRange> StatRanges;
};

UCLASS(CollapseCategories)
class UD1ItemFragment_Equippable : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EEquipmentType EquipmentType = EEquipmentType::Weapon;

	UPROPERTY(EditDefaultsOnly)
	FD1EquipmentAttachInfo AttachInfo;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, FStatRangeSet> Stats;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UD1AbilitySystemData> AbilitySystemToGrant;
};
