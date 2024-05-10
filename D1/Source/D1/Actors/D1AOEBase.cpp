#include "D1AOEBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueFunctionLibrary.h"
#include "Character/D1Character.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AOEBase)

AD1AOEBase::AD1AOEBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);

	HitBoxComponent = CreateDefaultSubobject<UBoxComponent>("HitBoxComponent");
	HitBoxComponent->SetupAttachment(GetRootComponent());

	HitBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AD1AOEBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BeginAOE);
}

void AD1AOEBase::Init(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle)
{
	DamageEffectSpecHandle = InDamageEffectSpecHandle;
}

void AD1AOEBase::BeginAOE()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(AOETimerHandle, this, &ThisClass::TickAOE, AttackIntervalTime, true, 0);
	}
}

void AD1AOEBase::TickAOE()
{
	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetActorLocation();
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(GetOwner(), GameplayCueTag, CueParameters);

	TArray<AActor*> OverlappingActors;
	HitBoxComponent->GetOverlappingActors(OverlappingActors, AD1Character::StaticClass());
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (GetOwner() == OverlappingActor)
			continue;
		
		AD1Character* OverlappingCharacter = Cast<AD1Character>(OverlappingActor);
		check(OverlappingCharacter);

		UAbilitySystemComponent* TargetASC = OverlappingCharacter->GetAbilitySystemComponent();
		check(TargetASC);

		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());

		if (HitActors.Contains(OverlappingCharacter) == false)
		{
			HitActors.Add(OverlappingCharacter);

			if (AdditionalGameplayEffectClass)
			{
				const UGameplayEffect* GameplayEffectCDO = AdditionalGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
				TargetASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, 1.f, TargetASC->MakeEffectContext());
			}
		}
	}

	CurrAttackCount++;
	
	if (CurrAttackCount == TargetAttackCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(AOETimerHandle);
		Destroy();
	}
}
