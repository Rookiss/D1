#include "D1AOEBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueFunctionLibrary.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AOEBase)

AD1AOEBase::AD1AOEBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetCollisionProfileName("OverlapOnlyPawn");
	BoxComponent->SetupAttachment(ArrowComponent);
}

void AD1AOEBase::BeginPlay()
{
	Super::BeginPlay();

	AttackIntervalTime = AttackTotalTime / TargetAttackCount;
	if (AttackIntervalTime <= 0)
	{
		Destroy();
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BeginAOE);
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
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(BoxComponent->Bounds.Origin, BoxComponent->Bounds.BoxExtent);
	GetWorld()->SpawnActor<AActor>(AOEElementClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	CurrentAttackCount++;
	
	if (CurrentAttackCount == TargetAttackCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(AOETimerHandle);
		Destroy();
	}
}
