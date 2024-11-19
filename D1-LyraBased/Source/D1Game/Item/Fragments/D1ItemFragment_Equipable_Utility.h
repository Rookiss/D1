#pragma once

#include "D1ItemFragment_Equipable_Attachment.h"
#include "D1ItemFragment_Equipable_Utility.generated.h"

class UGameplayEffect;

UCLASS()
class UD1ItemFragment_Equipable_Utility : public UD1ItemFragment_Equipable_Attachment
{
	GENERATED_BODY()

public:
	UD1ItemFragment_Equipable_Utility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EUtilityType UtilityType = EUtilityType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> UtilityEffectClass;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatSet> RarityStatSets;
};
