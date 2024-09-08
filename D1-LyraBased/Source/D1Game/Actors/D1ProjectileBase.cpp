#include "D1ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "D1WeaponBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/LyraCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ProjectileBase)

AD1ProjectileBase::AD1ProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	AActor::SetReplicateMovement(true);
	bNetLoadOnClient = false;
	bReplicates = true;

	AActor::SetLifeSpan(10.f);
	
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
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;
}

void AD1ProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* Character = Cast<ACharacter>(GetInstigator()))
	{
		SphereCollisionComponent->IgnoreActorWhenMoving(Character, true);
	}
	
	SphereCollisionComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::HandleComponentHit);
}

void AD1ProjectileBase::Destroyed()
{
	if (bHit == false && HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation());
	}
	
	if (HasAuthority() && OtherHitComponent.IsValid())
	{
		OtherHitComponent->OnComponentDeactivated.RemoveDynamic(this, &ThisClass::HandleOtherComponentDeactivated);
		OtherHitComponent = nullptr;
	}
	
	Super::Destroyed();
}

void AD1ProjectileBase::SetSpeed(float Speed)
{
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * Speed;
}

void AD1ProjectileBase::HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	if (bMultiComponentHit == false && bHit)
		return;
	
	bHit = true;
	SphereCollisionComponent->Deactivate();
	TrailNiagaraComponent->Deactivate();
	ProjectileMovementComponent->Deactivate();

	if (HasAuthority())
	{
		if (bAttachToHitComponent)
		{
			OtherHitComponent = OtherComponent;
			OtherComponent->OnComponentDeactivated.AddUniqueDynamic(this, &ThisClass::HandleOtherComponentDeactivated);
			AttachToComponent(OtherComponent, FAttachmentTransformRules::KeepWorldTransform, HitResult.BoneName);
		}
		
		AD1WeaponBase* TargetWeapon = Cast<AD1WeaponBase>(OtherActor);
		ALyraCharacter* TargetCharacter = Cast<ALyraCharacter>(OtherActor);
		if (TargetCharacter == nullptr)
		{
			TargetCharacter = Cast<ALyraCharacter>(OtherActor->GetOwner());
			OtherActor = TargetCharacter;
		}
		
		bool bBlockingHit = false;

		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
		UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());

		if (TargetWeapon)
		{
			bBlockingHit = true;
		}
		else if (TargetCharacter)
		{
			if (TargetASC && TargetASC->HasMatchingGameplayTag(D1GameplayTags::Status_Block))
			{
				FVector TargetLocation = OtherActor->GetActorLocation();
				FVector TargetDirection = OtherActor->GetActorForwardVector();
							
				FVector InstigatorLocation = GetInstigator()->GetActorLocation();
				FVector TargetToInstigator = InstigatorLocation - TargetLocation;
					
				float Degree = UKismetMathLibrary::DegAcos(TargetDirection.Dot(TargetToInstigator.GetSafeNormal()));
				if (Degree <= 45.f)
				{
					bBlockingHit = true;
				}
			}
		}
		
		if (SourceASC && HitGameplayCueTag.IsValid())
		{
			FGameplayCueParameters SourceCueParams;
			SourceCueParams.Location = HitResult.ImpactPoint;
			SourceCueParams.Normal = HitResult.ImpactNormal;
			SourceCueParams.PhysicalMaterial = bBlockingHit ? nullptr : HitResult.PhysMaterial;
			SourceASC->ExecuteGameplayCue(HitGameplayCueTag, SourceCueParams);
		}

		if (TargetASC && GetOwner() != OtherActor && GetOwner() != OtherComponent->GetOwner() && GetInstigator() != OtherActor)
		{
			const TSubclassOf<UGameplayEffect> DamageGEClass = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);

			FHitResult HitResultCopy = HitResult;
			HitResultCopy.bBlockingHit = bBlockingHit;
			
			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
			EffectContextHandle.AddHitResult(HitResultCopy);
			EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), this);

			FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageGEClass, 1.f, EffectContextHandle);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(D1GameplayTags::SetByCaller_BaseDamage, bBlockingHit ? Damage / 3.f : Damage);
			TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
	else
	{
		if (HitEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, HitResult.ImpactPoint);
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
