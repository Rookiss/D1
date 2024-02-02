#include "D1MonsterSet.h"

#include "D1GameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MonsterSet)

UD1MonsterSet::UD1MonsterSet()
	: Super()
{
	InitHealth(1.f);
	InitMaxHealth(1.f);

	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_Health, GetHealthAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MaxHealth, GetMaxHealthAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_Mana, GetManaAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MaxMana, GetMaxManaAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_AttackSpeed, GetAttackSpeedAttribute);
	TagToAttributeFunc.Add(D1GameplayTags::Attribute_Primary_MoveSpeed, GetMoveSpeedAttribute);
}

void UD1MonsterSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
}

bool UD1MonsterSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
		return false;

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.f)
		{
			if (Data.Target.HasMatchingGameplayTag(D1GameplayTags::ASC_DamageImmunity))
			{
				Data.EvaluatedData.Magnitude = 0.f;
				return false;
			}
		}
	}
	return true;
}

void UD1MonsterSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

	if (GetHealth() <= 0.f || bOutOfHealth == false)
	{
		OutOfHealthDelegate.Broadcast();
	}
	bOutOfHealth = (GetHealth() <= 0.f);
}

void UD1MonsterSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UD1MonsterSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UD1MonsterSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
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

	if (bOutOfHealth && GetHealth() > 0.f)
	{
		bOutOfHealth = false;
	}
}

void UD1MonsterSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
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
}

void UD1MonsterSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
	
	if (bOutOfHealth == false && GetHealth() <= 0.f)
	{
		OutOfHealthDelegate.Broadcast();
	}
	bOutOfHealth = (GetHealth() <= 0.f);
}

void UD1MonsterSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UD1MonsterSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UD1MonsterSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldValue);
}

void UD1MonsterSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeed, OldValue);
}

void UD1MonsterSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}
