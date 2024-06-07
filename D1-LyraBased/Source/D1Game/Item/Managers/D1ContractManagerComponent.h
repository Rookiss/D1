#pragma once

#include "Components/PlayerStateComponent.h"
#include "D1ContractManagerComponent.generated.h"

UCLASS()
class UD1ContractManagerComponent : public UPlayerStateComponent
{
	GENERATED_BODY()
	
public:
	UD1ContractManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddAllowedActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAllowedActor(AActor* Actor);

public:
	UFUNCTION(BlueprintCallable)
	bool IsAllowedActor(AActor* Actor) const;
	
private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AllowedActors;
};
