#include "D1ProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Physics/D1CollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ProjectileBase)

AD1ProjectileBase::AD1ProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SetRootComponent(SphereCollisionComponent);
	SphereCollisionComponent->SetCollisionObjectType(D1_ObjectChannel_Projectile);
	
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("UProjectileMovement");
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AD1ProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	SphereCollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleComponentBeginOverlap);
}

void AD1ProjectileBase::Destroyed()
{
	
	
	Super::Destroyed();
}

void AD1ProjectileBase::HandleComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}
