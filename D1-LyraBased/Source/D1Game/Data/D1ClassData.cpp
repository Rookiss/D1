#include "D1ClassData.h"

#include "Misc/DataValidation.h"
#include "System/LyraAssetManager.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassData)

const UD1ClassData& UD1ClassData::Get()
{
	return ULyraAssetManager::Get().GetClassData();
}

#if WITH_EDITOR
void UD1ClassData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ClassInfoSet.ClassInfoEntries.Sort([](const FD1ClassInfoEntry& A, const FD1ClassInfoEntry& B)
	{
		return A.ClassType < B.ClassType;
	});
}

EDataValidationResult UD1ClassData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	const int32 ClassCount = (int32)ECharacterClassType::Count;
	const TArray<FD1ClassInfoEntry>& ClassInfoEntries = ClassInfoSet.ClassInfoEntries;
	
	if (ClassInfoEntries.Num() != (int32)ECharacterClassType::Count)
	{
		Context.AddError(FText::FromString(TEXT("ClassInfoEntries.Num() != ECharacterClassType::Count")));
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		for (int i = 0; i < ClassCount; i++)
		{
			if (ClassInfoEntries[i].ClassType != (ECharacterClassType)i)
			{
				Context.AddError(FText::FromString(TEXT("ClassInfoEntries[i].ClassType != i")));
				Result = EDataValidationResult::Invalid;
			}
		}
	}
	
	return Result;
}
#endif // WITH_EDITOR

const FD1ClassInfoEntry& UD1ClassData::GetClassEntry(ECharacterClassType ClassType) const
{
	const TArray<FD1ClassInfoEntry>& ClassInfoEntries = ClassInfoSet.ClassInfoEntries;
	const int32 ClassIndex = (int32)ClassType;
	
	if (ClassInfoEntries.IsValidIndex(ClassIndex) == false)
	{
		static FD1ClassInfoEntry EmptyEntry;
		return EmptyEntry;
	}
	
	return ClassInfoEntries[ClassIndex];
}
