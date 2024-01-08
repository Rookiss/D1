#pragma once

#include "AttributeSet.h"
#include "GameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "D1UserWidget.generated.h"

#define BIND_ATTRIBUTE_CHANGED_SEPARATE_FUNCTION(PropertyName)		\
	On##PropertyName##Changed(AttributeSet->Get##PropertyName());	\
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->Get##PropertyName##Attribute()).AddLambda([this](const FOnAttributeChangeData& Data){ On##PropertyName##Changed(Data.NewValue); });

class UD1AttributeSet;
class UAbilitySystemComponent;

UCLASS()
class UD1UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UD1UserWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	void BindAbilityChangedDelegate();
	void BindAttributeChangedDelegates();

protected:
	void BindUnitedAttributeChangedDelegates();
	virtual void BindSeparatedAttributeChangedDelegates();
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttributeChanged(const FGameplayTag& Tag, float NewValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityChanged(bool bGiven, const FGameplayTag& AbilityTag);
	
protected:
	TMap<FGameplayTag, TSubclassOf<UD1AttributeSet>> WatchingAttributeTagToSetClass;

private:
	FDelegateHandle AbilityDelegateHandle;
	TArray<TKeyValuePair<const FGameplayAttribute, FDelegateHandle>> AttributeDelegateHandles;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<const UD1AttributeSet>> AttributeSets;
};
