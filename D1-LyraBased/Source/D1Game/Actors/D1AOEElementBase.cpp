#include "D1AOEElementBase.h"

#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AOEElementBase)

AD1AOEElementBase::AD1AOEElementBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	InitialLifeSpan = 2.f;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);
	
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCapsuleHalfHeight(88.f);
	CollisionComponent->SetCapsuleRadius(44.f);
	CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));
	CollisionComponent->bReturnMaterialOnMove = true;
	CollisionComponent->SetupAttachment(ArrowComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(CollisionComponent);
	
    TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
}

void AD1AOEElementBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ElementType == EAOEElementType::Projectile)
	{
		if (CurveData)
		{
			float Value = CurveData->GetFloatValue(0.f);
			FVector OldLocation = CollisionComponent->GetRelativeLocation();
			FVector NewLocation = FVector(OldLocation.X, OldLocation.Y, Value);
			CollisionComponent->SetRelativeLocation(NewLocation);

			CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
			
			FOnTimelineFloat TimelineTickCallback;
			TimelineTickCallback.BindDynamic(this, &ThisClass::OnTimelineTick);
		
			FOnTimelineEvent TimelineFinishedCallback;
			TimelineFinishedCallback.BindDynamic(this, &ThisClass::OnTimelineFinished);

			TimelineComponent->AddInterpFloat(CurveData, TimelineTickCallback);
			TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
		
			TimelineComponent->SetLooping(false);
			TimelineComponent->PlayFromStart();
		}
	}
	else if (ElementType == EAOEElementType::Explosion)
	{
		if (HasAuthority())
		{
			TArray<FHitResult> HitResults;
			FVector TraceLocation = CollisionComponent->GetComponentLocation();
			
			if (PerformTrace(TraceLocation, TraceLocation, HitResults))
			{
				TSet<AActor*> HitActors;
				for (const FHitResult& HitResult : HitResults)
				{
					AActor* HitActor = HitResult.GetActor();
					if (HitActor == nullptr || HitActors.Contains(HitActor))
						continue;

					HitActors.Add(HitActor);
					HandleCollisionDetection(HitResult);
				}
			}
		}

		CollisionComponent->Deactivate();
	}
}

void AD1AOEElementBase::OnTimelineTick(float Value)
{
	FHitResult HitResult;
	FVector OldLocation = CollisionComponent->GetRelativeLocation();
	FVector NewLocation = FVector(OldLocation.X, OldLocation.Y, Value);
	CollisionComponent->SetRelativeLocation(NewLocation, true, &HitResult);

	if (HitResult.GetActor())
	{
		if (HasAuthority())
		{
			HandleCollisionDetection(HitResult);
		}
		
		if (HitNiagaraEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagaraEffect, NiagaraComponent->GetComponentLocation());
		}
		
		TimelineComponent->Stop();
		CollisionComponent->Deactivate();
		NiagaraComponent->Deactivate();
	}
}

void AD1AOEElementBase::OnTimelineFinished()
{
	CollisionComponent->Deactivate();
	NiagaraComponent->Deactivate();
	
	if (HitNiagaraEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagaraEffect, NiagaraComponent->GetComponentLocation());
	}
}

bool AD1AOEElementBase::PerformTrace(const FVector& StartLocation, const FVector& EndLocation, TArray<FHitResult>& OutHitResults)
{
	TArray<AActor*> IgnoredActors = { GetOwner() };
	
	return UKismetSystemLibrary::SphereTraceMultiByProfile(GetWorld(),
		StartLocation + FVector(0.f, 0.f, CollisionComponent->GetScaledCapsuleHalfHeight()), EndLocation - FVector(0.f, 0.f, CollisionComponent->GetScaledCapsuleHalfHeight()),
		CollisionComponent->GetScaledCapsuleRadius(), TEXT("Projectile"),
		false, IgnoredActors, EDrawDebugTrace::None, OutHitResults, true, FLinearColor::Red, FLinearColor::Green, 5.f
	);
}

void AD1AOEElementBase::HandleCollisionDetection(const FHitResult& HitResult)
{
	if (HasAuthority() == false || HitResult.GetActor() == nullptr)
		return;

	if (HitResult.GetActor() == GetOwner() || HitResult.GetActor()->IsA<ALyraCharacter>() == false)
		return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitResult.GetActor());
	if (SourceASC == nullptr || TargetASC == nullptr)
		return;
		
	const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.AddHitResult(HitResult);
	EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), this);

	FGameplayEffectSpecHandle DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageGE, 1.f, EffectContextHandle);
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseDamage, Damage);
	TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());

	FGameplayCueParameters SourceCueParams;
	SourceCueParams.Location = HitResult.ImpactPoint;
	SourceCueParams.Normal = HitResult.ImpactNormal;
	SourceCueParams.PhysicalMaterial = HitResult.PhysMaterial;
	SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Weapon_Impact, SourceCueParams);

	if (AdditionalEffect)
	{
		FGameplayEffectSpecHandle AdditionalEffectSpecHandle = SourceASC->MakeOutgoingSpec(AdditionalEffect, 1.f, EffectContextHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*AdditionalEffectSpecHandle.Data.Get());
	}
}
