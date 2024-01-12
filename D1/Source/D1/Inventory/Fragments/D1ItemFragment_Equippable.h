#pragma once

#include "D1ItemFragment.h"
#include "D1ItemFragment_Equippable.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None,
	Head,
	Chest,
	Legs,
	Hands,
	Foot,
	
	Count	UMETA(Hidden)
};

UCLASS(BlueprintType, CollapseCategories)
class UD1ItemFragment_Equippable : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquipmentType EquipmentType = EEquipmentType::None;
};
