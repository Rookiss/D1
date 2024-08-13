#pragma once

#include "D1ItemFragment_Equippable_Attachment.h"
#include "D1ItemFragment_Equippable_Utility.generated.h"

UCLASS()
class UD1ItemFragment_Equippable_Utility : public UD1ItemFragment_Equippable_Attachment
{
	GENERATED_BODY()

public:
	UD1ItemFragment_Equippable_Utility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EUtilityType UtilityType = EUtilityType::Count;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatSet> RarityStatSets;
};
