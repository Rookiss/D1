#include "D1PocketWorldSubsystem.h"

#include "D1PocketStage.h"
#include "PocketLevel.h"
#include "PocketLevelInstance.h"
#include "PocketLevelSystem.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1PocketWorldSubsystem)

UPocketLevelInstance* UD1PocketWorldSubsystem::GetOrCreatePocketLevelFor(ULocalPlayer* LocalPlayer)
{
	UPocketLevelSubsystem* PocketLevelSubsystem = GetWorld()->GetSubsystem<UPocketLevelSubsystem>();
	
	UPocketLevel* PocketLevel = ULyraAssetManager::GetAssetByName<UPocketLevel>("PocketLevelData");
	FVector PocketLevelLocation = FVector(0, 0, -10000.f);

	return PocketLevelSubsystem->GetOrCreatePocketLevelFor(LocalPlayer, PocketLevel, PocketLevelLocation);
}

void UD1PocketWorldSubsystem::GetPocketStage(ULocalPlayer* LocalPlayer, FGetPocketStageDelegate Delegate)
{
	if (Delegate.IsBound() == false)
		return;

	if (CachedPocketStage.IsValid())
	{
		Delegate.Execute(CachedPocketStage.Get());
	}
	else
	{
		if (UPocketLevelInstance* PocketLevelInstance = GetOrCreatePocketLevelFor(LocalPlayer))
		{
			PocketLevelInstance->AddReadyCallback(FPocketLevelInstanceEvent::FDelegate::CreateLambda(
				[Delegate = MoveTemp(Delegate), this](UPocketLevelInstance* Instance)
				{
					Delegate.Execute(CachedPocketStage.Get());
				})
			);
		}
	}
}

void UD1PocketWorldSubsystem::SetPocketStage(AD1PocketStage* InPocketStage)
{
	CachedPocketStage = InPocketStage;
}
