#include "CrowActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

ACrowActor::ACrowActor()
{
	// Create the skeletal mesh component as the root
	CrowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrowMesh"));
	SetRootComponent(CrowMesh);

	// Create audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(CrowMesh);

	// Default values
	ActivationRadius = 600.0f;
	TriggerDelay = 1.0f;
	ClimbHeight = 200.0f;
	MaxFlightTime = 10.0f;
	DespawnDistance = 1000.0f;
	FlySpeed = 500.0f;
	bDestroyAfterFlight = true;

	// Set default state
	CurrentState = ECrowState::Idle;
	Timer = 0.0f;
	TargetPawn = nullptr;

	// Bind overlap event (we'll use a dynamic sphere for collision)
}

// Called when the game starts or when spawned
void ACrowActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialize with idle state
	if (CurrentState == ECrowState::Idle)
	{
		// Could play idle animation here
	}
}

// Called every frame
void ACrowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCrowFlight(DeltaTime);
}

void ACrowActor::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// No input setup needed for AI-controlled crow
}

void ACrowActor::TriggerFlee()
{
	if (CurrentState != ECrowState::Idle && CurrentState != ECrowState::Alarmed)
		return;

	CurrentState = ECrowState::Alarmed;
	Timer = 0.0f;
}

void ACrowActor::ResetToIdle()
{
	CurrentState = ECrowState::Idle;
	Timer = 0.0f;
	if (IsValid(CrowMesh))
	{
		CrowMesh->SetAnimationMode(EAnimationMode::AnimationCache);
	}
}

void ACrowActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int33 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if it's the player
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		TargetPawn = Pawn;
		TriggerFlee();
	}
}

void ACrowActor::UpdateCrowFlight(float DeltaTime)
{
	switch (CurrentState)
	{
	case ECrowState::Idle:
		// In idle, optionally rotate to face the player if visible
		break;

	case ECrowState::Alarmed:
		Timer += DeltaTime;
		if (Timer >= TriggerDelay)
		{
			CurrentState = ECrowState::TakingOff;
			Timer = 0.0f;

		// Play takeoff sound
			if (TakeOffSoundCue && AudioComponent)
			{
				AudioComponent->SetSound(TakeOffSoundCue);
				AudioComponent->Play();
			}

			// Start flying upward and away from player
			if (IsValid(TargetPawn))
			{
				FVector Direction = (GetActorLocation() - TargetPawn->GetActorLocation());
				Direction.Normalize();
				// Add some upward component
				Direction.Z = FMath::Max(0.5f, Direction.Z);
				// Set initial velocity
				CrowMesh->SetPhysicsLinearVelocity(Direction * FlySpeed * 0.5f);
			}
		}
		break;

	case ECrowState::TakingOff:
		Timer += DeltaTime;
		// Transition to flying after a short time
		if (Timer >= 0.5f)
		{
			CurrentState = ECrowState::Flying;
			Timer = 0.0f;
		}
		break;

	case ECrowState::Flying:
		Timer += DeltaTime;

		// Move forward based on flight direction
		if (IsValid(TargetPawn))
		{
			FVector ToPlayer = TargetPawn->GetActorLocation() - GetActorLocation();
			ToPlayer.Normalize();

			// Fly away from player with some leading
			FVector FlightDirection = -ToPlayer;
			FlightDirection.Z += FMath::FRandRange(0.1f, 0.3f); // Some upward angle
			FlightDirection.Normalize();

			// Apply velocity
			if (IsValid(CrowMesh))
			{
				CrowMesh->SetPhysicsLinearVelocity(FlightDirection * FlySpeed);
			}

			// Check if we've flown far enough
			float DistFromStart = FVector::Dist(GetActorLocation(), GetActorLocation()); // Would need to store start location
			if (Timer > MaxFlightTime || (IsValid(TargetPawn) && FVector::Dist(GetActorLocation(), TargetPawn->GetActorLocation()) > DespawnDistance))
			{
				CurrentState = ECrowState::Landing;
				Timer = 0.0f;
			}
		}
		break;

	case ECrowState::Landing:
		Timer += DeltaTime;
		// Simple landing - could play landing animation
		if (Timer >= 2.0f)
		{
			if (bDestroyAfterFlight)
			{
				Destroy();
			}
			else
			{
				ResetToIdle();
			}
		}
		break;
	}
}