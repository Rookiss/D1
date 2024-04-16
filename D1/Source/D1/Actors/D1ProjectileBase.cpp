#include "D1ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ProjectileBase)

AD1ProjectileBase::AD1ProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SetRootComponent(SphereCollisionComponent);
	SphereCollisionComponent->SetCollisionProfileName("Projectile");

	TrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("TrailNiagaraComponent");
	TrailNiagaraComponent->SetupAttachment(GetRootComponent());
	
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("UProjectileMovement");
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AD1ProjectileBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
}

void AD1ProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeSpan);
	TrailNiagaraComponent->SetAutoDestroy(true);
	SphereCollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleComponentBeginOverlap);
}

void AD1ProjectileBase::Destroyed()
{
	if (bHit == false && HasAuthority() == false)
	{
		bHit = true;

		TrailNiagaraComponent->Deactivate();
		TrailNiagaraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}
	
	Super::Destroyed();
}

void AD1ProjectileBase::HandleComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner() == OtherActor || GetOwner() == OtherComponent->GetOwner())
		return;
	
	if (bHit == false)
	{
		bHit = true;
		
		if (HasAuthority())
		{
			if (DamageEffectSpecHandle.Data.IsValid())
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
			}
			Destroy();
		}
		else
		{
			TrailNiagaraComponent->Deactivate();
			TrailNiagaraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
			
			SphereCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ProjectileMovementComponent->StopMovementImmediately();
		}
	}
}
