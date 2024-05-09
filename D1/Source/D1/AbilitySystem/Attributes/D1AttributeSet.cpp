#include "D1AttributeSet.h"

#include "D1GameplayTags.h"
#include "D1LogChannels.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AttributeSet)

UD1AttributeSet::UD1AttributeSet()
{
	InitHealth(1.f);
	InitMaxHealth(1.f);

	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_Health, GetHealthAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MaxHealth, GetMaxHealthAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_Mana, GetManaAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MaxMana, GetMaxManaAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_Stamina, GetStaminaAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MaxStamina, GetMaxStaminaAttribute);
	
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_AttackSpeed, GetAttackSpeedAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MoveSpeed, GetMoveSpeedAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MoveSpeedPercent, GetMoveSpeedPercentAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_PhysicalDamage, GetPhysicalDamageAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MagicalDamage, GetMagicalDamageAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_BaseDefense, GetBaseDefenseAttribute);

	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Strength, GetStrengthAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Will, GetWillAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Vigor, GetVigorAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Agility, GetAgilityAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Secondary_Knowledge, GetKnowledgeAttribute);
}

void UD1AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeedPercent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PhysicalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagicalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDefense, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Will, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Knowledge, COND_None, REPNOTIFY_Always);
}

bool UD1AttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
		return false;

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.f)
		{
			if (Data.Target.HasMatchingGameplayTag(D1GameplayTags::State_DamageImmunity))
			{
				Data.EvaluatedData.Magnitude = 0.f;
				return false;
			}
		}
	}
	return true;
}

void UD1AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetIncomingDamage(), 0.f, GetMaxHealth()));
		SetIncomingDamage(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingHealAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetIncomingHeal(), 0.f, GetMaxHealth()));
		SetIncomingHeal(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}

	if (GetHealth() <= 0.f && bOutOfHealth == false)
	{
		OnOutOfHealthDelegate.Broadcast();
		bOutOfHealth = true;
	}
}

void UD1AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UD1AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UD1AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			UD1AbilitySystemComponent* D1ASC = GetAbilitySystemComponent();
			check(D1ASC);

			D1ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		if (GetMana() > NewValue)
		{
			UD1AbilitySystemComponent* D1ASC = GetAbilitySystemComponent();
			check(D1ASC);

			D1ASC->ApplyModToAttribute(GetManaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		if (GetStamina() > NewValue)
		{
			UD1AbilitySystemComponent* D1ASC = GetAbilitySystemComponent();
			check(D1ASC);

			D1ASC->ApplyModToAttribute(GetStaminaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && GetHealth() > 0.f)
	{
		bOutOfHealth = false;
	}
}

void UD1AttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UD1AttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
	
	if (bOutOfHealth == false && GetHealth() <= 0.f)
	{
		OnOutOfHealthDelegate.Broadcast();
	}
	bOutOfHealth = (GetHealth() <= 0.f);
}

void UD1AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UD1AttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UD1AttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}

void UD1AttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina, OldValue);
}

void UD1AttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxStamina, OldValue);
}

void UD1AttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeed, OldValue);
}

void UD1AttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}

void UD1AttributeSet::OnRep_MoveSpeedPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeedPercent, OldValue);
}

void UD1AttributeSet::OnRep_PhysicalDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PhysicalDamage, OldValue);
}

void UD1AttributeSet::OnRep_MagicalDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagicalDamage, OldValue);
}

void UD1AttributeSet::OnRep_BaseDefense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseDefense, OldValue);
}

void UD1AttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Strength, OldValue);
}

void UD1AttributeSet::OnRep_Will(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Will, OldValue);
}

void UD1AttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Vigor, OldValue);
}

void UD1AttributeSet::OnRep_Agility(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Agility, OldValue);
}

void UD1AttributeSet::OnRep_Knowledge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Knowledge, OldValue);
}

TFunction<FGameplayAttribute()> UD1AttributeSet::GetAttributeFuncByTag(const FGameplayTag& AttributeTag) const
{
	if (const TFunction<FGameplayAttribute()>* FuncPtr = TagToAttributeFunc.Find(AttributeTag))
	{
		return *FuncPtr;
	}
	
	UE_LOG(LogD1AbilitySystem, Error, TEXT("Can't find Attribute Func By Tag [%s]."), *AttributeTag.ToString());
	return nullptr;
}

void UD1AttributeSet::GetAllAttributeTags(TArray<FGameplayTag>& OutAttributeTags) const
{
	TagToAttributeFunc.GetKeys(OutAttributeTags);
}

UD1AbilitySystemComponent* UD1AttributeSet::GetAbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
