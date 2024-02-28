#include "D1AssetManager.h"

#include "D1LogChannels.h"
#include "Data/D1AssetData.h"
#include "Data/D1ItemData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AssetManager)

UD1AssetManager::UD1AssetManager()
	: Super()
{
    
}

UD1AssetManager& UD1AssetManager::Get()
{
	check(GEngine);

	if (UD1AssetManager* Singleton = Cast<UD1AssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogD1Asset, Fatal, TEXT("Can't find D1AssetManager"));

	return *NewObject<UD1AssetManager>();
}

void UD1AssetManager::Initialize()
{
	Get().LoadPreloadAssets();
}

const UD1ItemData* UD1AssetManager::GetItemData()
{
	return GetAssetByName<UD1ItemData>("ItemData");
}

void UD1AssetManager::GetAssetByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate)
{
	if (AssetPath.IsValid())
	{
		if (UObject* LoadedAsset = AssetPath.ResolveObject())
		{
			CompletedDelegate.ExecuteIfBound(AssetPath.GetAssetFName(), LoadedAsset);
		}
		else
		{
			LoadAsyncByPath(AssetPath, CompletedDelegate);
		}
	}
}

void UD1AssetManager::LoadSyncByPath(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		UObject* LoadedAsset = AssetPath.ResolveObject();
		if (LoadedAsset == nullptr)
		{
			if (UAssetManager::IsInitialized())
			{
				LoadedAsset = UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
			}
			else
			{
				LoadedAsset = AssetPath.TryLoad();
			}
		}
	
		if (LoadedAsset)
		{
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
		}
		else
		{
			UE_LOG(LogD1Asset, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
		}
	}
}

void UD1AssetManager::LoadSyncByName(const FName& AssetName)
{
	UD1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);
	
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if (AssetPath.IsValid())
	{
		UObject* LoadedAsset = AssetPath.ResolveObject();
		if (LoadedAsset == nullptr)
		{
			if (UAssetManager::IsInitialized())
			{
				LoadedAsset = UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
			}
			else
			{
				LoadedAsset = AssetPath.TryLoad();
			}
		}
	
		if (LoadedAsset)
		{
			Get().AddLoadedAsset(AssetName, LoadedAsset);
		}
		else
		{
			UE_LOG(LogD1Asset, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
		}
	}
}

void UD1AssetManager::LoadSyncByLabel(const FName& Label)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogD1Asset, Error, TEXT("AssetManager must be initialized"));
		return;
	}
	
	UD1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	TArray<FSoftObjectPath> AssetPaths;
	
	const FAssetSet& AssetSet = AssetData->GetAssetSetByLabel(Label);
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		if (AssetPath.IsValid())
		{
			AssetPaths.Emplace(AssetPath);
		}
	}
	
	GetStreamableManager().RequestSyncLoad(AssetPaths);
	
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		if (AssetPath.IsValid())
		{
			if (UObject* LoadedAsset = AssetPath.ResolveObject())
			{
				Get().AddLoadedAsset(AssetEntry.AssetName, LoadedAsset);
			}
			else
			{
				UE_LOG(LogD1Asset, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
			}
		}
	}
}

void UD1AssetManager::LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogD1Asset, Error, TEXT("AssetManager must be initialized"));
		return;
	}
	
	if (AssetPath.IsValid())
	{
		if (UObject* LoadedAsset = AssetPath.ResolveObject())
		{
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
		}
		else
		{
			TArray<FSoftObjectPath> AssetPaths;
			AssetPaths.Add(AssetPath);
			
			TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPaths);
			Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([AssetName = AssetPath.GetAssetFName(), AssetPath, CompleteDelegate = MoveTemp(CompletedDelegate)]()
			{
				UObject* LoadedAsset = AssetPath.ResolveObject();
				Get().AddLoadedAsset(AssetName, LoadedAsset);
				if (CompleteDelegate.IsBound())
					CompleteDelegate.Execute(AssetName, LoadedAsset);
			}));
		}
	}
}

void UD1AssetManager::LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompletedDelegate)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogD1Asset, Error, TEXT("AssetManager must be initialized"));
		return;
	}
	
	UD1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);
	
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if (AssetPath.IsValid())
	{
		if (UObject* LoadedAsset = AssetPath.ResolveObject())
		{
			Get().AddLoadedAsset(AssetName, LoadedAsset);
		}
		else
		{
			TArray<FSoftObjectPath> AssetPaths;
			AssetPaths.Add(AssetPath);
			
			TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPaths);
			Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([AssetName, AssetPath, CompleteDelegate = MoveTemp(CompletedDelegate)]()
			{
				UObject* LoadedAsset = AssetPath.ResolveObject();
				Get().AddLoadedAsset(AssetName,LoadedAsset);
				if (CompleteDelegate.IsBound())
					CompleteDelegate.Execute(AssetName, LoadedAsset);
			}));
		}
	}
}

void UD1AssetManager::LoadAsyncByLabel(const FName& Label, FAsyncLoadCompletedDelegate CompletedDelegate, FAsyncLoadUpdateDelegate UpdateDelegate)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogD1Asset, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	UD1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	TArray<FSoftObjectPath> AssetPaths;
	
	const FAssetSet& AssetSet = AssetData->GetAssetSetByLabel(Label);
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		if (AssetPath.IsValid())
		{
			AssetPaths.Emplace(AssetPath);
		}
	}
	
	TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPaths);

	Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([&AssetEntries = AssetSet.AssetEntries, Label, CompleteDelegate = MoveTemp(CompletedDelegate)]()
	{
		for (const FAssetEntry& AssetEntry : AssetEntries)
		{
			const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
			if (AssetPath.IsValid())
			{
				if (UObject* LoadedAsset = AssetPath.ResolveObject())
				{
					Get().AddLoadedAsset(AssetEntry.AssetName, LoadedAsset);
				}
				else
				{
					UE_LOG(LogD1Asset, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
				}
			}
		}

		if (CompleteDelegate.IsBound())
			CompleteDelegate.Execute(Label, nullptr);
	}));
	
	Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateLambda([UpdateDelegate = MoveTemp(UpdateDelegate)](TSharedRef<FStreamableHandle> HandleRef)
	{
		if (UpdateDelegate.IsBound())
			UpdateDelegate.Execute(HandleRef->GetProgress());
	}));
}

void UD1AssetManager::ReleaseByPath(const FSoftObjectPath& AssetPath)
{
	UD1AssetManager& AssetManager = Get();
	FScopeLock LoadedAssetsLock(&AssetManager.LoadedAssetsCritical);
	FName AssetName = AssetPath.GetAssetFName();
	
	if (AssetManager.NameToLoadedAsset.Contains(AssetName))
	{
		AssetManager.NameToLoadedAsset.Remove(AssetName);
	}
	else
	{
		UE_LOG(LogD1Asset, Log, TEXT("Can't find loaded asset by assetName [%s]."), *AssetName.ToString());
	}
}

void UD1AssetManager::ReleaseByName(const FName& AssetName)
{
	UD1AssetManager& AssetManager = Get();
	FScopeLock LoadedAssetsLock(&AssetManager.LoadedAssetsCritical);
	if (AssetManager.NameToLoadedAsset.Contains(AssetName))
	{
		AssetManager.NameToLoadedAsset.Remove(AssetName);
	}
	else
	{
		UE_LOG(LogD1Asset, Log, TEXT("Can't find loaded asset by assetName [%s]."), *AssetName.ToString());
	}
}

void UD1AssetManager::ReleaseByLabel(const FName& Label)
{
	UD1AssetManager& AssetManager = Get();
	UD1AssetData* LoadedAssetData = AssetManager.LoadedAssetData;
	const FAssetSet& AssetSet = LoadedAssetData->GetAssetSetByLabel(Label);

	FScopeLock LoadedAssetsLock(&AssetManager.LoadedAssetsCritical);
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FName& AssetName = AssetEntry.AssetName;
		if (AssetManager.NameToLoadedAsset.Contains(AssetName))
		{
			AssetManager.NameToLoadedAsset.Remove(AssetName);
		}
		else
		{
			UE_LOG(LogD1Asset, Log, TEXT("Can't find loaded asset by assetName [%s]."), *AssetName.ToString());
		}
	}
}

void UD1AssetManager::ReleaseAll()
{
	UD1AssetManager& AssetManager = Get();
	FScopeLock LoadedAssetsLock(&AssetManager.LoadedAssetsCritical);
	AssetManager.NameToLoadedAsset.Reset();
}

void UD1AssetManager::LoadPreloadAssets()
{
	if (LoadedAssetData)
		return;
	
	UD1AssetData* AssetData = nullptr;
	FPrimaryAssetType PrimaryAssetType(UD1AssetData::StaticClass()->GetFName());
	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete(0.f, false);
		AssetData = Cast<UD1AssetData>(Handle->GetLoadedAsset());
	}
	
	if (AssetData)
	{
		LoadedAssetData = AssetData;
		LoadSyncByLabel("Preload");
	}
	else
	{
		UE_LOG(LogD1Asset, Fatal, TEXT("Failed to load AssetData asset type [%s]."), *PrimaryAssetType.ToString());
	}
}

void UD1AssetManager::AddLoadedAsset(const FName& AssetName, const UObject* Asset)
{
	if (AssetName.IsValid() && Asset)
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		if (NameToLoadedAsset.Contains(AssetName) == false)
		{
			NameToLoadedAsset.Add(AssetName, Asset);
		}
	}
}
