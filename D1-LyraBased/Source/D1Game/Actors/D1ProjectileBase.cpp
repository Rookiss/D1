#include "D1ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "D1WeaponBase.h"
#include "NiagaraComponent.h"
#include "Character/LyraCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/LyraAssetManager.h"

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
			
			ALyraCharacter* TargetCharacter = Cast<ALyraCharacter>(OtherActor);
			AD1WeaponBase* TargetWeapon = Cast<AD1WeaponBase>(OtherActor);
			
			if (TargetCharacter == nullptr)
			{
				TargetCharacter = Cast<ALyraCharacter>(OtherActor->GetOwner());
				OtherActor = TargetCharacter;
			}

			bool bIsBlockHit = false;

			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
			UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());

			if (TargetWeapon)
			{
				bIsBlockHit = true;
			}
			else if (TargetCharacter)
			{
				if (TargetASC)
				{
					if (TargetASC->HasMatchingGameplayTag(D1GameplayTags::Status_Block))
					{
						FVector TargetLocation = OtherActor->GetActorLocation();
						FVector TargetDirection = OtherActor->GetActorForwardVector();
								
						FVector InstigatorLocation = GetInstigator()->GetActorLocation();
						FVector TargetToInstigator = InstigatorLocation - TargetLocation;
						
						float Degree = UKismetMathLibrary::DegAcos(TargetDirection.Dot(TargetToInstigator.GetSafeNormal()));
						if (Degree <= 45.f)
						{
							bIsBlockHit = true;
						}
					}
				}
			}
			else
			{
				bIsBlockHit = true;
			}

			if (SourceASC)
			{
				FGameplayCueParameters SourceCueParams;
				SourceCueParams.Location = HitResult.ImpactPoint;
				SourceCueParams.Normal = HitResult.ImpactNormal;
				SourceCueParams.PhysicalMaterial = bIsBlockHit ? nullptr : HitResult.PhysMaterial;
				SourceASC->ExecuteGameplayCue(D1GameplayTags::GameplayCue_Impact_Weapon, SourceCueParams);
			}

			if (TargetASC && bIsBlockHit == false && DamageEffectSpecHandle.Data.IsValid())
			{
				FHitResult HitResultCopy = HitResult;
					
				FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
				HitResultCopy.bBlockingHit = bIsBlockHit;
				EffectContextHandle.AddHitResult(HitResultCopy);
				EffectContextHandle.AddInstigator(GetInstigator(), this);
				DamageEffectSpecHandle.Data->SetContext(EffectContextHandle);
					
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
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
