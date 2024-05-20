#include "D1ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Physics/LyraCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ProjectileBase)

AD1ProjectileBase::AD1ProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	AActor::SetReplicateMovement(true);
	bNetLoadOnClient = false;
	bReplicates = true;
	
	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereCollisionComponent");
	SphereCollisionComponent->SetCollisionProfileName("Projectile");
	SphereCollisionComponent->SetupAttachment(GetRootComponent());
	SphereCollisionComponent->bReturnMaterialOnMove = true;
	SetRootComponent(SphereCollisionComponent);

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMeshComponent");
	ProjectileMeshComponent->SetCollisionProfileName("NoCollision");
	ProjectileMeshComponent->SetupAttachment(SphereCollisionComponent);
	
	TrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("TrailNiagaraComponent");
	TrailNiagaraComponent->SetupAttachment(ProjectileMeshComponent);
	
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->MaxSpeed = 2000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;
}

void AD1ProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollisionComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::HandleComponentHit);
}

void AD1ProjectileBase::Destroyed()
{
	if (HasAuthority())
	{
		if (OtherHitComponent.IsValid())
		{
			OtherHitComponent->OnComponentDeactivated.RemoveDynamic(this, &ThisClass::HandleOtherComponentDeactivated);
			OtherHitComponent = nullptr;
		}
	}
	else
	{
		if (bHit == false)
		{
			bHit = true;
		
			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
			}

			if (ImpactEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
			}
		}
	}
	
	Super::Destroyed();
}

void AD1ProjectileBase::Init(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle)
{
	DamageEffectSpecHandle = InDamageEffectSpecHandle;
}

void AD1ProjectileBase::HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	if (GetOwner() == OtherActor || GetOwner() == OtherComponent->GetOwner() || GetInstigator() == OtherActor)
		return;
	
	if (bHit == false)
	{
		bHit = true;
		TrailNiagaraComponent->Deactivate();
		
		if (HasAuthority())
		{
			OtherHitComponent = OtherComponent;
			ProjectileMovementComponent->Deactivate();
			
			OtherComponent->OnComponentDeactivated.AddUniqueDynamic(this, &ThisClass::HandleOtherComponentDeactivated);
			AttachToComponent(OtherComponent, FAttachmentTransformRules::KeepWorldTransform, HitResult.BoneName);
			
			if (HitResult.PhysMaterial.IsValid() && HitResult.PhysMaterial->SurfaceType == D1_PhysicalMaterial_Flesh && DamageEffectSpecHandle.Data.IsValid())
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
				{
					// TODO: Team Check
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
			}
		}
		else
		{
			if (GameplayCueTag.IsValid())
			{
				FGameplayCueParameters CueParameters;
				CueParameters.Location = HitResult.ImpactPoint;
				CueParameters.Normal = HitResult.ImpactNormal;
				CueParameters.PhysicalMaterial = HitResult.PhysMaterial;
				
				UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), GameplayCueTag, CueParameters);
			}

			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
			}

			if (ImpactEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
			}
		}
	}
}

void AD1ProjectileBase::HandleOtherComponentDeactivated(UActorComponent* OtherComponent)
{
	if (HasAuthority())
	{
		Destroy();
	}
}
