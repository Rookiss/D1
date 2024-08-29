#include "D1GameplayAbilityTargetActor_GroundTrace.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbilityTargetActor_GroundTrace)

AD1GameplayAbilityTargetActor_GroundTrace::AD1GameplayAbilityTargetActor_GroundTrace(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CollisionHeightOffset = 0.0f;
}

void AD1GameplayAbilityTargetActor_GroundTrace::StartTargeting(UGameplayAbility* InAbility)
{
	//CollisionShape starts as "line," which is correct as a default for our purposes
	if (CollisionRadius > 0.0f)
	{
		//Check CollisionHeight here because the shape code doesn't. It is used as half-height
		if ((CollisionHeight * 0.5f) > CollisionRadius)
		{
			CollisionShape = FCollisionShape::MakeCapsule(CollisionRadius, CollisionHeight * 0.5f);
			CollisionHeightOffset = CollisionHeight * 0.5f;
		}
		else
		{
			CollisionShape = FCollisionShape::MakeSphere(CollisionRadius);
			CollisionHeight = 0.0f;
			CollisionHeightOffset = CollisionRadius;
		}
	}
	else
	{
		//Make sure these are clean.
		CollisionRadius = CollisionHeight = 0.0f;
	}
	Super::StartTargeting(InAbility);
}

bool AD1GameplayAbilityTargetActor_GroundTrace::AdjustCollisionResultForShape(const FVector OriginalStartPoint, const FVector OriginalEndPoint, const FCollisionQueryParams Params, FHitResult& OutHitResult) const
{
	UWorld *ThisWorld = GetWorld();
	//Pull back toward player to find a better spot, accounting for the width of our object
	FVector Movement = (OriginalEndPoint - OriginalStartPoint);	
	FVector MovementDirection = Movement.GetSafeNormal();
	float MovementMagnitude2D = Movement.Size2D();

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		if (CollisionShape.ShapeType == ECollisionShape::Capsule)
		{
			DrawDebugCapsule(ThisWorld, OriginalEndPoint, CollisionHeight * 0.5f, CollisionRadius, FQuat::Identity, FColor::Black);
		}
		else
		{
			DrawDebugSphere(ThisWorld, OriginalEndPoint, CollisionRadius, 8, FColor::Black);
		}
	}
#endif // ENABLE_DRAW_DEBUG

	if (MovementMagnitude2D <= (CollisionRadius * 2.0f))
	{
		return false;		//Bad case!
	}

	//TODO This increment value needs to ramp up - the first few increments should be small, then we should start moving in larger steps. A few ideas for this:
	//1. Use a curve! Even one defined by a hardcoded formula would be fine, this isn't something that should require user tuning, or that the user should really know/care about.
	//2. Use larger increments as the object is further from the player/camera, since the user can't really perceive precision at long range.
	float IncrementSize = FMath::Clamp<float>(CollisionRadius * 0.5f, 20.0f, 50.0f);
	float LerpIncrement = IncrementSize / MovementMagnitude2D;
	FHitResult LocalResult;
	FVector TraceStart;
	FVector TraceEnd;
	for (float LerpValue = CollisionRadius / MovementMagnitude2D; LerpValue < 1.0f; LerpValue += LerpIncrement)
	{
		TraceEnd = TraceStart = OriginalEndPoint - (LerpValue * Movement);
		TraceEnd.Z -= 99999.0f;
		SweepWithFilter(LocalResult, ThisWorld, Filter, TraceStart, TraceEnd, FQuat::Identity, CollisionShape, TraceProfile.Name, Params);
		if (!LocalResult.bStartPenetrating)
		{
			if (!LocalResult.bBlockingHit || (LocalResult.HitObjectHandle.IsValid() && LocalResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass())))
			{
				//Off the map, or hit an actor
#if ENABLE_DRAW_DEBUG
				if (bDebug)
				{
					if (CollisionShape.ShapeType == ECollisionShape::Capsule)
					{
						DrawDebugCapsule(ThisWorld, LocalResult.Location, CollisionHeight * 0.5f, CollisionRadius, FQuat::Identity, FColor::Yellow);
					}
					else
					{
						DrawDebugSphere(ThisWorld, LocalResult.Location, CollisionRadius, 8, FColor::Yellow);
					}
				}
#endif // ENABLE_DRAW_DEBUG
				continue;
			}
#if ENABLE_DRAW_DEBUG
			if (bDebug)
			{
				if (CollisionShape.ShapeType == ECollisionShape::Capsule)
				{
					DrawDebugCapsule(ThisWorld, LocalResult.Location, CollisionHeight * 0.5f, CollisionRadius, FQuat::Identity, FColor::Green);
				}
				else
				{
					DrawDebugSphere(ThisWorld, LocalResult.Location, CollisionRadius, 8, FColor::Green);
				}
			}
#endif // ENABLE_DRAW_DEBUG

			//TODO: Test for flat ground. Concept: Test four corners and the center, make triangles out of the center and adjacent corner points. Check normal.Z of triangles against a minimum Z value.

			OutHitResult = LocalResult;
			return true;
		}
#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			if (CollisionShape.ShapeType == ECollisionShape::Capsule)
			{
				DrawDebugCapsule(ThisWorld, TraceStart, CollisionHeight * 0.5f, CollisionRadius, FQuat::Identity, FColor::Red);
			}
			else
			{
				DrawDebugSphere(ThisWorld, TraceStart, CollisionRadius, 8, FColor::Red);
			}
		}
#endif // ENABLE_DRAW_DEBUG
	}
	return false;
}

bool AD1GameplayAbilityTargetActor_GroundTrace::IsConfirmTargetingAllowed()
{
	return bLastTraceWasGood;
}

FHitResult AD1GameplayAbilityTargetActor_GroundTrace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_GroundTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(InSourceActor);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();// InSourceActor->GetActorLocation();
	FVector TraceEnd;
	AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);		//Effective on server and launching client only

	// ------------------------------------------------------

	FHitResult ReturnHitResult;
	//Use a line trace initially to see where the player is actually pointing
	LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
	//Default to end of trace line if we don't hit anything.
	if (!ReturnHitResult.bBlockingHit)
	{
		ReturnHitResult.Location = TraceEnd;
	}

	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 0.1f, 0, 0.1f);
	
	FNavLocation NavLocation;
	if (UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(ReturnHitResult.Location, NavLocation, FVector(88.0f, 88.0f, 2000.0f)))
	{
		ReturnHitResult.Location = NavLocation.Location;
		// DrawDebugSphere(GetWorld(), ReturnHitResult.Location, 8.0f, 8, FColor::Blue, false, 0.1f, 0, 0.1f);
	}
	
	// TraceStart = ReturnHitResult.Location - (TraceEnd - TraceStart).GetSafeNormal();
	// TraceEnd = TraceStart;
	// TraceStart.Z += CollisionHeightOffset;
	// TraceEnd.Z -= 99999.0f;
	// LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
	
	bLastTraceWasGood = true;
	
	if (CollisionShape.ShapeType != ECollisionShape::Line)
	{
		ReturnHitResult.Location.Z += CollisionHeightOffset;
		TraceStart = InSourceActor->GetActorLocation();
		TraceEnd = ReturnHitResult.Location;
		TraceStart.Z += CollisionHeightOffset;
		bLastTraceWasGood = AdjustCollisionResultForShape(TraceStart, TraceEnd, Params, ReturnHitResult);
		if (bLastTraceWasGood)
		{
			ReturnHitResult.Location.Z -= CollisionHeightOffset;
		}
	}

	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		LocalReticleActor->SetIsTargetValid(bLastTraceWasGood);
		LocalReticleActor->SetActorLocation(ReturnHitResult.Location);
	}
	
	ReturnHitResult.TraceStart = StartLocation.GetTargetingTransform().GetLocation();

	return ReturnHitResult;
}
