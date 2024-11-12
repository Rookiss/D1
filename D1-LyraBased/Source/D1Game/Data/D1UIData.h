#pragma once

#include "D1Define.h"
#include "GameplayTagContainer.h"
#include "D1UIData.generated.h"

class UImage;
class UD1SkillStatHoverWidget;
class UD1InventoryValidWidget;
class UD1ItemDragWidget;
class UD1ItemHoverWidget;
class UD1EquipmentEntryWidget;
class UD1InventorySlotWidget;
class UD1InventoryEntryWidget;

USTRUCT(BlueprintType)
struct FD1ItemRarityInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	FColor Color = FColor::Black;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> EntryTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> HoverTexture;
};

USTRUCT(BlueprintType)
struct FD1ItemRarityInfoSet
{
	GENERATED_BODY()

public:
	FD1ItemRarityInfoSet();

public:
	UTexture2D* GetEntryTexture(EItemRarity ItemRarity) const;
	UTexture2D* GetHoverTexture(EItemRarity ItemRarity) const;
	FColor GetRarityColor(EItemRarity ItemRarity) const;
	
public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FD1ItemRarityInfoEntry> RarityInfoEntries;
};

USTRUCT(BlueprintType)
struct FD1UIInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly)
	FText Content;
};

UCLASS(BlueprintType, Const, meta=(DisplayName="D1 UI Data"))
class UD1UIData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1UIData& Get();

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
public:
	UTexture2D* GetEntryRarityTexture(EItemRarity ItemRarity) const;
	UTexture2D* GetHoverRarityTexture(EItemRarity ItemRarity) const;
	FColor GetRarityColor(EItemRarity ItemRarity) const;

	const FD1UIInfo& GetTagUIInfo(FGameplayTag Tag) const;

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint UnitInventorySlotSize = FIntPoint::ZeroValue;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1ItemDragWidget> DragWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1ItemHoverWidget> ItemHoverWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1SkillStatHoverWidget> SkillStatHoverWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1EquipmentEntryWidget> EquipmentEntryWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1InventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1InventoryEntryWidget> InventoryEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1InventoryValidWidget> InventoryValidWidgetClass;
	
private:
	UPROPERTY(EditDefaultsOnly)
	FD1ItemRarityInfoSet RarityInfoSet;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Tag UI Infos"))
	TMap<FGameplayTag, FD1UIInfo> TagUIInfos;
};
