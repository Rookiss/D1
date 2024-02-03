#include "D1AssetData.h"

#include "System/D1AssetManager.h"
#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AssetData)

void UD1AssetData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	AssetNameToPath.Empty();
	AssetLabelToSet.Empty();

	AssetGroupNameToSet.KeySort([](const FName& A, const FName& B)
	{
		return (A.Compare(B) < 0);
	});
	
	for (const auto& Pair : AssetGroupNameToSet)
	{
		const FAssetSet& AssetSet = Pair.Value;
		for (FAssetEntry AssetEntry : AssetSet.AssetEntries)
		{
			FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
			const FString& AssetName = AssetPath.GetAssetName();
			if (AssetName.StartsWith(TEXT("BP_")) || AssetName.StartsWith(TEXT("B_")) || AssetName.StartsWith(TEXT("WBP_")) ||
				AssetName.StartsWith(TEXT("GE_")) || AssetName.StartsWith(TEXT("GA_")) ||  AssetName.StartsWith(TEXT("ABP_")))
			{
				FString AssetPathString = AssetPath.GetAssetPathString();
				AssetPathString.Append(TEXT("_C"));
				AssetPath = FSoftObjectPath(AssetPathString);
			}
			
			AssetNameToPath.Emplace(AssetEntry.AssetName, AssetEntry.AssetPath);
			for (const FName& Label : AssetEntry.AssetLabels)
			{
				AssetLabelToSet.FindOrAdd(Label).AssetEntries.Emplace(AssetEntry);
			}
		}
	}
}

#if WITH_EDITOR
EDataValidationResult UD1AssetData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const auto& Pair : AssetGroupNameToSet)
	{
		const FAssetSet& AssetSet = Pair.Value;
		for (int32 i = 0; i < AssetSet.AssetEntries.Num(); i++)
		{
			const FAssetEntry& AssetEntry = AssetSet.AssetEntries[i];
			if (AssetEntry.AssetName.IsNone())
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Asset Name is None : [Group Name : %s] - [Entry Index : %d]"), *Pair.Key.ToString(), i)));
				Result = EDataValidationResult::Invalid;
			}

			if (AssetEntry.AssetPath.IsValid() == false)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Asset Path is Invalid : [Group Name : %s] - [Entry Index : %d]"), *Pair.Key.ToString(), i)));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif // #if WITH_EDITOR

FSoftObjectPath UD1AssetData::GetAssetPathByName(const FName& AssetName)
{
	FSoftObjectPath* AssetPath = AssetNameToPath.Find(AssetName);
	ensureAlwaysMsgf(AssetPath, TEXT("Can't find Asset Path from Asset Name [%s]."), *AssetName.ToString());
	return *AssetPath;
}

const FAssetSet& UD1AssetData::GetAssetSetByLabel(const FName& Label)
{
	const FAssetSet* AssetSet = AssetLabelToSet.Find(Label);
	ensureAlwaysMsgf(AssetSet, TEXT("Can't find Asset Set from Label [%s]."), *Label.ToString());
	return *AssetSet;
}
