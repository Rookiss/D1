#pragma once

#include "GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "D1UserWidget.generated.h"

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
	
	void BindAttributeChangedDelegates();
	void UnbindAttributeChangedDelegates();

protected:
	template <class UserClass, typename FuncType>
	void BindAttributeChangedDelegate(const FGameplayTag& AttributeTag, TSubclassOf<UD1AttributeSet> AttributeSetClass, UserClass* Object, FuncType Func);
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityChanged(bool bIsGiven, const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnAttributeChanged(const FGameplayTag& AttributeTag, float NewValue);

protected:
	TArray<TPair<FGameplayTag, TSubclassOf<UD1AttributeSet>>> WatchingAttributes;
	
private:
	FDelegateHandle AbilityDelegateHandle;
	TArray<TTuple<FGameplayTag, TSubclassOf<UD1AttributeSet>, FDelegateHandle>> AttributeDelegateHandles;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	bool bShouldBindAbilityDelegate = false;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBindAttributeDelegate = false;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};

template <class UserClass, typename FuncType>
void UD1UserWidget::BindAttributeChangedDelegate(const FGameplayTag& AttributeTag, TSubclassOf<UD1AttributeSet> AttributeSetClass, UserClass* Object, FuncType Func)
{
	if (const UD1AttributeSet* AttributeSet = Cast<UD1AttributeSet>(AbilitySystemComponent->GetAttributeSet(AttributeSetClass)))
	{
		if (const auto& AttributeFunc = AttributeSet->GetAttributeFuncByTag(AttributeTag))
		{
			const FGameplayAttribute& Attribute = AttributeFunc();
			(Object->*Func)(Attribute.GetNumericValue(AttributeSet));
			FDelegateHandle Handle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
			[Object, Func](const FOnAttributeChangeData& Data) { (Object->*Func)(Data.NewValue); });
			AttributeDelegateHandles.Emplace(AttributeTag, AttributeSetClass, Handle);
		}
	}
}
