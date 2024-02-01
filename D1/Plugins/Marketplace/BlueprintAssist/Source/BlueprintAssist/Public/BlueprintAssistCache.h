// Copyright 2021 fpwong. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SGraphPin.h"
#include "BlueprintAssistGlobals.h"

#include "BlueprintAssistCache.generated.h"

USTRUCT()
struct BLUEPRINTASSIST_API FBANodeData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector2D CachedNodeSize = FVector2D::ZeroVector;

	UPROPERTY()
	TMap<FGuid, float> CachedPins; // pin guid -> pin offset

	UPROPERTY()
	bool bLocked = false;

	UPROPERTY()
	FGuid NodeGroup;

	UPROPERTY()
	TArray<FGuid> NodeGroups;

	void ResetSize()
	{
		CachedNodeSize = FVector2D::ZeroVector;
		CachedPins.Reset();
	}

	bool HasSize() const
	{
		return !CachedNodeSize.IsZero();
	}
};

USTRUCT()
struct BLUEPRINTASSIST_API FBAGraphData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TMap<FGuid, FBANodeData> NodeData; // node guid -> node data

	void CleanupGraph(UEdGraph* Graph);

	FBANodeData& GetNodeData(UEdGraphNode* Node);

	bool bTriedLoadingMetaData = false;
};

USTRUCT()
struct BLUEPRINTASSIST_API FBAPackageData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TMap<FGuid, FBAGraphData> GraphData; // graph guid -> graph data
};

USTRUCT()
struct BLUEPRINTASSIST_API FBACacheData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TMap<FName, FBAPackageData> PackageData; // package name -> package data

	UPROPERTY()
	TArray<FString> BookmarkedFolders;

	UPROPERTY()
	int CacheVersion = -1;
};

class BLUEPRINTASSIST_API FBACache
{
public:
	static FBACache& Get();
	static void TearDown();

	void Init();

	FBACacheData& GetCacheData() { return CacheData; }

	void LoadCache();

	void SaveCache();

	void DeleteCache();

	void CleanupFiles();

	FBAGraphData& GetGraphData(UEdGraph* Graph);

	FString GetProjectSavedCachePath(bool bFullPath = false);
	FString GetPluginCachePath(bool bFullPath = false);
	FString GetCachePath(bool bFullPath = false);
	FString GetAlternateCachePath(bool bFullPath = false);

	void SaveGraphDataToPackageMetaData(UEdGraph* Graph);
	bool LoadGraphDataFromPackageMetaData(UEdGraph* Graph, FBAGraphData& GraphData);
	void ClearPackageMetaData(UEdGraph* Graph);

	void SetBookmarkedFolder(const FString& FolderPath, int Index)
	{
		if (Index >= CacheData.BookmarkedFolders.Num())
		{
			CacheData.BookmarkedFolders.SetNum(Index + 1);
		}

		CacheData.BookmarkedFolders[Index] = FolderPath;
	}

	TOptional<FString> FindBookmarkedFolder(int Index)
	{
		return CacheData.BookmarkedFolders.IsValidIndex(Index) ? CacheData.BookmarkedFolders[Index] : TOptional<FString>();
	}

private:
	bool bHasLoaded = false;

	FBACacheData CacheData;

	bool bHasSavedThisFrame = false;
	bool bHasSavedMetaDataThisFrame = false;

#if BA_UE_VERSION_OR_LATER(5, 0)
	void OnObjectPreSave(UObject* Object, FObjectPreSaveContext Context);
#endif

	void OnObjectSaved(UObject* Object);

	void ResetSavedThisFrame()
	{
		bHasSavedThisFrame = false;
		bHasSavedMetaDataThisFrame = false;
	}
};
