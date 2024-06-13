#include "D1UIPocketWorldSubsystem.h"

#include "PocketLevel.h"
#include "PocketLevelSystem.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1UIPocketWorldSubsystem)

void UD1UIPocketWorldSubsystem::SpawnPocketLevel(ULocalPlayer* LocalPlayer)
{
	UPocketLevelSubsystem* PocketLevelSubsystem = GetWorld()->GetSubsystem<UPocketLevelSubsystem>();
	UPocketLevel* PocketLevel = ULyraAssetManager::GetAssetByName<UPocketLevel>("PocketLevelData");
	FVector PocketLevelLocation = FVector(0, 0, -10000);
	PocketLevelSubsystem->GetOrCreatePocketLevelFor(LocalPlayer, PocketLevel, PocketLevelLocation);
}
