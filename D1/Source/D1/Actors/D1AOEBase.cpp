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
	// bReplicates = true;

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

void AD1AOEBase::BeginAOE()
{
	if (HasAuthority() == false)
		return;

	GetWorld()->GetTimerManager().SetTimer(AOETimerHandle, this, &ThisClass::TickAOE, AttackIntervalTime, true, 0);
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
		if (OverlappingCharacter == nullptr)
			continue;

		UAbilitySystemComponent* TargetASC = OverlappingCharacter->GetAbilitySystemComponent();
		if (TargetASC == nullptr)
			continue;

		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
	}

	CurrAttackCount++;
	
	if (CurrAttackCount == TargetAttackCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(AOETimerHandle);
		Destroy();
	}
}
