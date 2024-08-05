#pragma once

#include "D1Interactable.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CanInteraction(const FD1InteractionQuery& InteractionQuery) const override;
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractActiveStarted(AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteractActiveStarted")
	void K2_OnInteractActiveStarted(AActor* Interactor);
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractActiveEnded(AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteractActiveEnded")
	void K2_OnInteractActiveEnded(AActor* Interactor);
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionSuccess(AActor* Interactor);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteractionSuccess")
	void K2_OnInteractionSuccess(AActor* Interactor);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	bool bCanUsed = false;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsUsed = false;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CachedInteractors;
};
