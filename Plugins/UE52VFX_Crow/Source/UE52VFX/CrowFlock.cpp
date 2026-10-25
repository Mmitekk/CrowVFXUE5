// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowFlock.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "CrowActor.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

ACrowFlock::ACrowFlock()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpawnSpline = CreateDefaultSubobject<USplineComponent>(TEXT("SpawnSpline"));
	SpawnSpline->SetupAttachment(Root);
	SpawnSpline->SetClosedLoop(false);

	// A simple line to start with; edit the points to shape the flock.
	SpawnSpline->ClearSplinePoints(false);
	SpawnSpline->AddSplinePoint(FVector(0.0f, -350.0f, 0.0f), ESplineCoordinateSpace::Local, false);
	SpawnSpline->AddSplinePoint(FVector(0.0f, 350.0f, 0.0f), ESplineCoordinateSpace::Local, false);
	SpawnSpline->UpdateSpline();
}

// ---------------------------------------------------------------------------- construction

void ACrowFlock::ApplyBirdDefaults(int32 NewCount)
{
	const int32 OldCount = Birds.Num();
	if (OldCount == NewCount)
	{
		return;
	}

	Birds.SetNum(NewCount);
	for (int32 Index = OldCount; Index < NewCount; ++Index)
	{
		FCrowFlockBird& Bird = Birds[Index];
		// Spread the new birds evenly around the flock and stagger them a little.
		Bird.FlightYaw = (NewCount > 0) ? (360.0f * static_cast<float>(Index) / static_cast<float>(NewCount)) : 0.0f;
		Bird.Delay = 0.06f * static_cast<float>(Index);
		Bird.bCaw = true;
		Bird.Scale = 0.0f;
	}
}

void ACrowFlock::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BirdCount = FMath::Clamp(BirdCount, 1, 64);
	ApplyBirdDefaults(BirdCount);
}

void ACrowFlock::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnBirds();
	}

	if (bScareOnBeginPlay)
	{
		GetWorldTimerManager().SetTimer(AutoScareTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			ScareAll(nullptr);
		}), FMath::Max(StartDelay, 0.01f), false);
	}
}

void ACrowFlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bScared || ScareRadius <= 0.0f)
	{
		return;
	}

	const APawn* Pawn = FindScarePawn();
	if (Pawn != nullptr && FVector::Dist(Pawn->GetActorLocation(), GetActorLocation()) <= ScareRadius)
	{
		ScareAll(const_cast<APawn*>(Pawn));
	}
}

APawn* ACrowFlock::FindScarePawn() const
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	return (PC != nullptr) ? PC->GetPawn() : nullptr;
}

// ---------------------------------------------------------------------------- spawning

void ACrowFlock::SpawnBirds()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	SpawnedCrows.RemoveAll([](const TObjectPtr<ACrowActor>& Crow) { return Crow == nullptr; });
	if (SpawnedCrows.Num() > 0)
	{
		return;
	}

	ApplyBirdDefaults(BirdCount);

	UClass* ClassToSpawn = (CrowClass != nullptr) ? CrowClass.Get() : ACrowActor::StaticClass();

	const float SplineLength = (SpawnSpline != nullptr) ? SpawnSpline->GetSplineLength() : 0.0f;
	const int32 NumBirds = Birds.Num();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 Index = 0; Index < NumBirds; ++Index)
	{
		const FCrowFlockBird& Bird = Birds[Index];

		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		if (SpawnSpline != nullptr && SplineLength > 0.0f)
		{
			const float Alpha = (NumBirds > 1) ? (static_cast<float>(Index) / static_cast<float>(NumBirds - 1)) : 0.5f;
			float Distance = Alpha * SplineLength;
			if (SplinePlacementJitter > 0.0f)
			{
				Distance += FMath::FRandRange(-SplinePlacementJitter, SplinePlacementJitter);
			}
			Distance = FMath::Clamp(Distance, 0.0f, SplineLength);

			SpawnLocation = SpawnSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			SpawnRotation = SpawnSpline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		}

		// Small vertical offset so no two birds occupy the exact same spot.
		SpawnLocation.Z += FMath::FRandRange(0.0f, 4.0f);

		ACrowActor* Crow = World->SpawnActor<ACrowActor>(ClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		if (Crow == nullptr)
		{
			continue;
		}

		// The flock drives the take-off, so the bird must not scare itself on proximity,
		// and its idle caw is optional per flock.
		Crow->bUseProximityTrigger = false;
		Crow->bCawWhileIdle = !bMuteIdleCaw;
		Crow->SetActorScale3D(FVector(Bird.Scale > 0.0f ? Bird.Scale : CrowScale));

		SpawnedCrows.Add(Crow);
	}

	OnFlockSpawned();
}

void ACrowFlock::RespawnFlock()
{
	GetWorldTimerManager().ClearTimer(AutoScareTimer);

	for (const TObjectPtr<ACrowActor>& Crow : SpawnedCrows)
	{
		if (Crow != nullptr)
		{
			Crow->Destroy();
		}
	}
	SpawnedCrows.Reset();
	bScared = false;

	SpawnBirds();
}

int32 ACrowFlock::GetSpawnedBirdCount() const
{
	int32 Count = 0;
	for (const TObjectPtr<ACrowActor>& Crow : SpawnedCrows)
	{
		if (Crow != nullptr)
		{
			++Count;
		}
	}
	return Count;
}

// ---------------------------------------------------------------------------- scare

void ACrowFlock::ScareAll(AActor* InstigatorActor)
{
	if (bScared)
	{
		return;
	}
	bScared = true;
	GetWorldTimerManager().ClearTimer(AutoScareTimer);

	OnFlockScared(InstigatorActor);

	// The bird entries may have been edited after the spawn, so make sure the array matches.
	ApplyBirdDefaults(BirdCount);

	for (int32 Index = 0; Index < SpawnedCrows.Num(); ++Index)
	{
		ACrowActor* Crow = SpawnedCrows[Index];
		if (Crow == nullptr)
		{
			continue;
		}

		const FCrowFlockBird& Bird = Birds.IsValidIndex(Index) ? Birds[Index] : FCrowFlockBird();

		float Yaw = Bird.FlightYaw;
		if (bRandomizeDirections && DirectionSpread > 0.0f)
		{
			Yaw += FMath::FRandRange(-DirectionSpread, DirectionSpread);
		}

		const FVector Direction = FRotator(0.0f, Yaw, 0.0f).Vector();
		const float Delay = FMath::Max(BaseScareDelay + Bird.Delay, 0.0f);

		Crow->ScareInDirection(InstigatorActor, Direction, Delay, Bird.bCaw);
	}
}

// ---------------------------------------------------------------------------- blueprint events

void ACrowFlock::OnFlockSpawned_Implementation()
{
}

void ACrowFlock::OnFlockScared_Implementation(AActor* InstigatorActor)
{
}
