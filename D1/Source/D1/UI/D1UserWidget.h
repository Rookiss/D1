#pragma once

#include "GameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "D1UserWidget.generated.h"

#define BIND_ATTRIBUTE_CHANGED_DELEGATE(AttributeSetClassName, AttributeTag, PropertyName)												\
	if (##AttributeSetClassName##* AttributeSet = const_cast<##AttributeSetClassName##*>(Cast<##AttributeSetClassName##>(				\
		AbilitySystemComponent->GetAttributeSet(##AttributeSetClassName##::StaticClass())))) 											\
	{																																	\
		FGameplayTag Tag = AttributeTag;																								\
		float CurrentValue = AttributeSet->Get##PropertyName##();																		\
		On##PropertyName##Changed(nullptr, CurrentValue, CurrentValue);																	\
		FDelegateHandle Handle = AttributeSet->##PropertyName##ChangedDelegate.AddUObject(this, &ThisClass::On##PropertyName##Changed);	\
		AttributeDelegateHandles.Emplace(Tag, Handle);																					\
	}		
		
#define UNBIND_ATTRIBUTE_CHANGED_DELEGATE(AttributeSetClassName, AttributeTag, PropertyName)											\
	FGameplayTag Tag = AttributeTag;																									\
	if (FDelegateHandle* Handle = AttributeDelegateHandles.Find(Tag))																	\
	{																																	\
		if (##AttributeSetClassName##* AttributeSet = const_cast<##AttributeSetClassName##*>(Cast<##AttributeSetClassName##>(			\
			AbilitySystemComponent->GetAttributeSet(##AttributeSetClassName##::StaticClass())))) 										\
		{																																\
			AttributeSet->##PropertyName##ChangedDelegate.Remove(*Handle);																\
		}																																\
	}

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
	void UnbindAbilityChangedDelegate();

protected:
	virtual void BindAttributeChangedDelegates();
	virtual void UnbindAttributeChangedDelegates();
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityChanged(bool bIsGiven, const FGameplayTag& AbilityTag);

private:
	FDelegateHandle AbilityDelegateHandle;
	TMap<FGameplayTag, FDelegateHandle> AttributeDelegateHandles;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
