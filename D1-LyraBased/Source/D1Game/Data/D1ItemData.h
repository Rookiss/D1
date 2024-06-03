#pragma once

#include "D1ItemData.generated.h"

UCLASS(BlueprintType, Const, meta=(DisplayName="D1 Item Data"))
class UD1ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UD1ItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
public:
	const UD1ItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	int32 FindItemTemplateIDByClass(TSubclassOf<UD1ItemTemplate> ItemTemplateClass) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<UD1ItemTemplate>> ItemTemplateIDToClass;

	UPROPERTY()
	TMap<TSubclassOf<UD1ItemTemplate>, int32> ItemTemplateClassToID;
};
