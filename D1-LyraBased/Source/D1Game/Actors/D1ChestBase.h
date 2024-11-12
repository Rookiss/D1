#pragma once

#include "D1Define.h"
#include "Interaction/D1WorldInteractable.h"
#include "D1ChestBase.generated.h"

class UD1ItemTemplate;
class UArrowComponent;
class UD1InventoryManagerComponent;

UENUM(BlueprintType)
enum class EItemAddType : uint8
{
	None,
	Weapon,
	Armor,
	Custom
};

USTRUCT(BlueprintType)
struct FItemAddRule
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemAddType ItemAddType = EItemAddType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemAddType == EItemAddType::Custom", EditConditionHides))
	TArray<TSubclassOf<UD1ItemTemplate>> CustomItemTemplateClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ItemAddTypeRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EItemRarity> ItemRarities;
};

UENUM(BlueprintType)
enum class EChestState : uint8
{
	Open,
	Close
};

UCLASS()
class AD1ChestBase : public AD1WorldInteractable
{
	GENERATED_BODY()
	
public:
	AD1ChestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetChestState(EChestState NewChestState);
	
private:
	UFUNCTION()
	void OnRep_ChestState();
	
protected:
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_ChestState)
	EChestState ChestState = EChestState::Close;
	
	UPROPERTY(EditDefaultsOnly, Category="Info")
	FD1InteractionInfo OpenedInteractionInfo;
	
	UPROPERTY(EditDefaultsOnly, Category="Info")
	FD1InteractionInfo ClosedInteractionInfo;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	TObjectPtr<UAnimMontage> OpenMontage;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	TObjectPtr<UAnimMontage> CloseMontage;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Info")
	TArray<FItemAddRule> ItemAddRules;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	bool bShouldFallback = false;

	UPROPERTY(EditDefaultsOnly, Category="Info", meta=(EditCondition="bShouldFallback", EditConditionHides))
	TSubclassOf<UD1ItemTemplate> FallbackItemTemplateClass;

	UPROPERTY(EditDefaultsOnly, Category="Info", meta=(EditCondition="bShouldFallback", EditConditionHides))
	EItemRarity FallbackItemItemRarity = EItemRarity::Poor;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManager;
};
