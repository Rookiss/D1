#pragma once

#include "Item/D1ItemInstance.h"
#include "Interaction/D1WorldInteractable.h"
#include "D1ChestBase.generated.h"

class UD1ItemTemplate;
class UArrowComponent;

UENUM(BlueprintType)
enum class EChestState : uint8
{
	Open,
	Close
};

USTRUCT(BlueprintType)
struct FItemAddRule
{
	GENERATED_BODY()

public:
	FItemAddRule();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UD1ItemTemplate> ItemTemplateClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FD1ItemRarityProbability> ItemRarityProbabilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount = 1;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Info")
	TArray<FItemAddRule> ItemAddRules;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1InventoryManagerComponent> InventoryManager;
};
