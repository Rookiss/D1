#include "D1ElectricFieldPhaseData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ElectricFieldPhaseData)

const UD1ElectricFieldPhaseData& UD1ElectricFieldPhaseData::Get()
{
	return ULyraAssetManager::Get().GetElectricFieldPhaseData();
}

const FD1ElectricFieldPhaseEntry& UD1ElectricFieldPhaseData::GetPhaseEntry(int32 PhaseIndex) const
{
	if (PhaseIndex < 0 || PhaseIndex >= PhaseEntries.Num())
	{
		static FD1ElectricFieldPhaseEntry EmptyEntry;
		return EmptyEntry;
	}

	return PhaseEntries[PhaseIndex];
}
