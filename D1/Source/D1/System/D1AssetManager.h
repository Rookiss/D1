#pragma once

#include "D1LogChannels.h"
#include "Data/D1AssetData.h"
#include "Engine/AssetManager.h"
#include "D1AssetManager.generated.h"

class UD1InputData;
class UD1ItemData;
struct FD1ItemTemplate;

DECLARE_DELEGATE_OneParam(FAsyncLoadUpdateDelegate, float/*Progress*/);
DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName&/*AssetName or Label*/, UObject*/*LoadedAsset*/);

UCLASS()
class UD1AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	UD1AssetManager();
	
	static UD1AssetManager& Get();

public:
	static void Initialize();
	
	static const UD1ItemData* GetItemData();
	static const FD1ItemTemplate& GetItemTemplate(int32 TemplateID);

	static const UD1InputData* GetInputData();
	
	static void GetAssetByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	
	template<typename AssetType>
	static AssetType* GetAssetByName(const FName& AssetName);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclassByName(const FName& AssetName);

	static void LoadSyncByPath(const FSoftObjectPath& AssetPath);
	static void LoadSyncByName(const FName& AssetName);
	static void LoadSyncByLabel(const FName& Label);

	static void LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	static void LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	static void LoadAsyncByLabel(const FName& Label, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate(), FAsyncLoadUpdateDelegate UpdateDelegate = FAsyncLoadUpdateDelegate());

	static void ReleaseByPath(const FSoftObjectPath& AssetPath);
	static void ReleaseByName(const FName& AssetName);
	static void ReleaseByLabel(const FName& Label);
	static void ReleaseAll();
	
private:
	void LoadPreloadAssets();
	void AddLoadedAsset(const FName& AssetName, const UObject* Asset);

private:
	UPROPERTY()
	TObjectPtr<UD1AssetData> LoadedAssetData;
	
	UPROPERTY()
	TMap<FName, TObjectPtr<const UObject>> NameToLoadedAsset;
	
	FCriticalSection LoadedAssetsCritical;
};

template<typename AssetType>
AssetType* UD1AssetManager::GetAssetByName(const FName& AssetName)
{
	UD1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());
		if (LoadedAsset == nullptr)
		{
			UE_LOG(LogD1Asset, Warning, TEXT("Attempted sync loading because asset hadn't loaded yet [%s]."), *AssetPath.ToString());
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}
	return LoadedAsset;
}

template<typename ClassType>
TSubclassOf<ClassType> UD1AssetManager::GetSubclassByName(const FName& AssetName)
{
	UD1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	TSubclassOf<ClassType> LoadedSubclass = nullptr;
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if (AssetPath.IsValid())
	{
		LoadedSubclass = Cast<UClass>(AssetPath.ResolveObject());
		if (LoadedSubclass == nullptr)
		{
			UE_LOG(LogD1Asset, Warning, TEXT("Attempted sync loading because asset hadn't loaded yet [%s]."), *AssetPath.ToString());
			LoadedSubclass = Cast<UClass>(AssetPath.TryLoad());
		}
	}
	return LoadedSubclass;
}
