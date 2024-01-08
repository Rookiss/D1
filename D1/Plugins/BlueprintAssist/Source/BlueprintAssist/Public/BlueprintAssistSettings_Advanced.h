#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintAssistSettings_Advanced.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class BLUEPRINTASSIST_API UBASettings_Advanced final : public UObject
{
	GENERATED_BODY()

public:
	UBASettings_Advanced(const FObjectInitializer& ObjectInitializer);

	/* If swapping produced any looping wires, remove them */
	UPROPERTY(EditAnywhere, config, Category = "Commands|Swap Nodes")
	bool bRemoveLoopingCausedBySwapping;

	UPROPERTY(EditAnywhere, config, Category = "Commands")
	TSet<FName> DisabledCommands;

	/* Potential issue where pins can get stuck in a hovered state on the material graph */
	UPROPERTY(EditAnywhere, config, Category = "Material Graph|Experimental")
	bool bEnableMaterialGraphPinHoverFix;

	/* Fix for issue where copy-pasting material nodes will result in their material expressions having the same GUID */
	UPROPERTY(EditAnywhere, config, Category = "Material Graph|Experimental", DisplayName="Generate Unique GUID For Material Expressions")
	bool bGenerateUniqueGUIDForMaterialExpressions;

	/* Instead of making a json file to store cache data, store it in the blueprint's package meta data */
	UPROPERTY(EditAnywhere, config, Category = "Cache|Experimental")
	bool bStoreCacheDataInPackageMetaData;

	/* Save cache file JSON in a more human-readable format. Useful for debugging, but increases size of cache files.  */
	UPROPERTY(EditAnywhere, config, Category = "Cache")
	bool bPrettyPrintCacheJSON;

	/* Use a custom blueprint action menu for creating nodes (very prototype, not supported in 5.0 or earlier) */
	UPROPERTY(EditAnywhere, config, Category = "Misc|Experimental")
	bool bUseCustomBlueprintActionMenu;

	/* Hacky workaround to ensure that default comment nodes will be correctly resized after formatting */
	UPROPERTY(EditAnywhere, config, Category = "Misc|Experimental")
	bool bForceRefreshGraphAfterFormatting;

	FORCEINLINE static const UBASettings_Advanced& Get() { return *GetDefault<UBASettings_Advanced>(); }
	FORCEINLINE static UBASettings_Advanced& GetMutable() { return *GetMutableDefault<UBASettings_Advanced>(); }
};
