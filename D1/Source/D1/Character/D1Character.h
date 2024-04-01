#pragma once

#include "Data/D1AbilitySystemData.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "D1Character.generated.h"

class UD1AttributeSet;
class UD1AbilitySystemComponent;

UCLASS()
class AD1Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AD1Character(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitAbilitySystem();
	virtual void ApplyAbilitySystemData(const FName& DataName);

public:
	void HandleOutOfHealth();
	virtual void StartDeath();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UD1AbilitySystemComponent* GetD1AbilitySystemComponent() const;
	const UD1AttributeSet* GetAttributeSet() { return AttributeSet; }
	
protected:
	UPROPERTY()
	FD1AbilitySystemData_GrantedHandles GrantedHandles;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UD1AttributeSet> AttributeSet;
};
