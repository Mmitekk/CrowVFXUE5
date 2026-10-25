// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowActor.h"

#include "Animation/AnimSequence.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

ACrowActor::ACrowActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CrowRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CrowRoot"));
	SetRootComponent(CrowRoot);

	CrowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrowMesh"));
	CrowMesh->SetupAttachment(CrowRoot);
	CrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CrowMesh->SetGenerateOverlapEvents(false);
	CrowMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	CrowAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CrowAudio"));
	CrowAudio->SetupAttachment(CrowMesh);
	CrowAudio->bAutoActivate = false;
	CrowAudio->bOverrideAttenuation = false;
	CrowAudio->bAllowSpatialization = true;
}

// ---------------------------------------------------------------------------- lifecycle

void ACrowActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyMeshSettings();
}

void ACrowActor::BeginPlay()
{
	Super::BeginPlay();

	IdleTransform = GetActorTransform();
	ApplyMeshSettings();

	State = ECrowState::Idle;
	PlayIdle();
}

void ACrowActor::ApplyMeshSettings()
{
	if (CrowMesh == nullptr)
	{
		return;
	}

	if (CrowSkeletalMesh != nullptr)
	{
		CrowMesh->SetSkeletalMesh(CrowSkeletalMesh);
	}
	if (CrowMaterialOverride != nullptr)
	{
		CrowMesh->SetMaterial(0, CrowMaterialOverride);
	}

	CrowMesh->SetRelativeLocation(MeshRelativeLocation);
	CrowMesh->SetRelativeRotation(MeshRelativeRotation);
	CrowMesh->SetRelativeScale3D(FVector(CrowScale));
}

void ACrowActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDrawActivationRadius && GetWorld() != nullptr)
	{
		const FColor Color = (State == ECrowState::Idle) ? FColor(40, 220, 90) : FColor(220, 60, 40);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ActivationRadius, 24, Color, false, 0.0f, 0, 1.5f);
	}

	switch (State)
	{
	case ECrowState::Idle:
		UpdateIdle(DeltaSeconds);
		break;
	case ECrowState::Flying:
		UpdateFlying(DeltaSeconds);
		break;
	default:
		break;
	}
}

// ---------------------------------------------------------------------------- idle

APawn* ACrowActor::FindReactionPawn() const
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	if (bOnlyReactToPlayer)
	{
		const APlayerController* PC = World->GetFirstPlayerController();
		return (PC != nullptr) ? PC->GetPawn() : nullptr;
	}

	APawn* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (TActorIterator<APawn> It(const_cast<UWorld*>(World)); It; ++It)
	{
		const float DistSq = FVector::DistSquared(It->GetActorLocation(), GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = *It;
		}
	}
	return Best;
}

float ACrowActor::GetDistanceToActivationTarget() const
{
	const APawn* Pawn = FindReactionPawn();
	return (Pawn != nullptr) ? FVector::Dist(Pawn->GetActorLocation(), GetActorLocation()) : -1.0f;
}

void ACrowActor::UpdateIdle(float DeltaSeconds)
{
	const APawn* Pawn = FindReactionPawn();
	if (Pawn == nullptr)
	{
		return;
	}

	const FVector ToPawn = Pawn->GetActorLocation() - GetActorLocation();
	const float Distance = ToPawn.Size();

	if (bUseProximityTrigger && Distance <= ActivationRadius)
	{
		TriggerFlee(const_cast<APawn*>(Pawn));
		return;
	}

	if (bLookAtPlayer && Distance <= LookAtRadius && LookAtTurnSpeed > 0.0f)
	{
		FRotator Desired = ToPawn.Rotation();
		Desired.Pitch = 0.0f;
		Desired.Roll = 0.0f;
		const FRotator NewRot = FMath::RInterpConstantTo(GetActorRotation(), Desired, DeltaSeconds, LookAtTurnSpeed);
		SetActorRotation(NewRot);
	}
}

// ---------------------------------------------------------------------------- reaction

void ACrowActor::PlayRandomSound(const TArray<TObjectPtr<USoundBase>>& Sounds, float Volume)
{
	if (Sounds.Num() == 0 || CrowAudio == nullptr)
	{
		return;
	}

	int32 Index = FMath::RandRange(0, Sounds.Num() - 1);
	for (int32 Attempt = 0; Attempt < Sounds.Num() && Sounds[Index] == nullptr; ++Attempt)
	{
		Index = (Index + 1) % Sounds.Num();
	}

	USoundBase* Sound = Sounds[Index];
	if (Sound == nullptr)
	{
		return;
	}

	CrowAudio->SetSound(Sound);
	CrowAudio->SetVolumeMultiplier(FMath::Max(Volume, 0.0f));
	CrowAudio->bAllowSpatialization = bSpatializeSounds;
	if (SoundAttenuation != nullptr)
	{
		CrowAudio->bOverrideAttenuation = false;
		CrowAudio->AttenuationSettings = SoundAttenuation;
	}
	CrowAudio->Play();
}

void ACrowActor::TriggerFlee(AActor* InstigatorActor)
{
	if (State != ECrowState::Idle)
	{
		return;
	}

	AlarmInstigator = InstigatorActor;
	State = ECrowState::Alarmed;

	PlayRandomSound(AlarmedSounds, AlarmedSoundVolume);
	OnCrowAlarmed(InstigatorActor);

	if (TriggerDelay <= 0.0f)
	{
		EnterTakeOff();
	}
	else
	{
		GetWorldTimerManager().SetTimer(StateTimer, this, &ACrowActor::EnterTakeOff, TriggerDelay, false);
	}
}

void ACrowActor::PlayAnimationSequence(UAnimSequence* Anim, bool bLoop, float Rate)
{
	if (CrowMesh == nullptr || Anim == nullptr)
	{
		return;
	}

	CrowMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	CrowMesh->PlayAnimation(Anim, bLoop);
	CrowMesh->SetPlayRate(Rate);
}

void ACrowActor::PlayIdle()
{
	PlayAnimationSequence(IdleAnimation, true, IdlePlayRate);
}

void ACrowActor::EnterTakeOff()
{
	if (State != ECrowState::Alarmed)
	{
		return;
	}

	State = ECrowState::TakingOff;
	OnCrowTakeOff();

	if (CrowMesh != nullptr)
	{
		if (bDisableCollisionOnTakeOff)
		{
			CrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		PlayAnimationSequence(TakeOffAnimation, false, TakeOffAnimationTimeScale);
	}

	PlayRandomSound(TakeOffSounds, TakeOffSoundVolume);

	// Escape direction: away from whoever scared the bird, with a bit of random spread.
	FVector Direction = GetActorForwardVector();
	if (bFlyAwayFromPlayer)
	{
		const AActor* Threat = AlarmInstigator.Get();
		const FVector ThreatLocation = (Threat != nullptr) ? Threat->GetActorLocation() : GetActorLocation() - GetActorForwardVector() * 500.0f;
		Direction = GetActorLocation() - ThreatLocation;
	}
	Direction.Z = 0.0f;
	if (Direction.IsNearlyZero())
	{
		Direction = GetActorForwardVector();
	}
	Direction = Direction.GetSafeNormal();
	if (DirectionRandomSpreadDeg > 0.0f)
	{
		Direction = Direction.RotateAngleAxis(FMath::FRandRange(-DirectionRandomSpreadDeg, DirectionRandomSpreadDeg), FVector::UpVector);
	}
	FlightDirection = Direction;

	FlightStartLocation = GetActorLocation();
	FlightElapsed = 0.0f;
	Velocity = Direction * (FlySpeed * 0.25f) + FVector(0.0f, 0.0f, TakeOffVerticalSpeed);

	const float TakeOffLength = (TakeOffAnimation != nullptr)
		? TakeOffAnimation->GetPlayLength() / FMath::Max(TakeOffAnimationTimeScale, 0.01f)
		: 0.5f;
	GetWorldTimerManager().SetTimer(StateTimer, this, &ACrowActor::EnterFlying, FMath::Max(TakeOffLength * 0.6f, 0.1f), false);
}

void ACrowActor::EnterFlying()
{
	if (State != ECrowState::TakingOff)
	{
		return;
	}

	State = ECrowState::Flying;
	OnCrowFlyAway();

	PlayAnimationSequence(FlyAnimation, true, FlyPlayRate);
}

// ---------------------------------------------------------------------------- flight

void ACrowActor::UpdateFlying(float DeltaSeconds)
{
	FlightElapsed += DeltaSeconds;

	const float HeightGain = GetActorLocation().Z - FlightStartLocation.Z;
	const float TargetVerticalSpeed = (HeightGain < ClimbHeight) ? ClimbSpeed : 0.0f;
	Velocity.Z = FMath::FInterpConstantTo(Velocity.Z, TargetVerticalSpeed, DeltaSeconds, FlyAcceleration);

	FVector Horizontal(Velocity.X, Velocity.Y, 0.0f);
	const float HorizontalSpeed = Horizontal.Size();
	const float NewSpeed = FMath::FInterpConstantTo(HorizontalSpeed, FlySpeed, DeltaSeconds, FlyAcceleration);
	const FVector HorizontalDir = (HorizontalSpeed > KINDA_SMALL_NUMBER) ? (Horizontal / HorizontalSpeed) : FlightDirection;
	Velocity = HorizontalDir * NewSpeed + FVector(0.0f, 0.0f, Velocity.Z);

	AddActorWorldOffset(Velocity * DeltaSeconds, false);

	if (!Velocity.IsNearlyZero())
	{
		const FRotator DesiredRotation = Velocity.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaSeconds, 3.5f));
	}

	const float Travelled = FVector::Dist(GetActorLocation(), FlightStartLocation);
	const bool bOutOfTime = FlightElapsed >= MaxFlightTime;
	const bool bTooFar = DespawnDistance > 0.0f && Travelled >= DespawnDistance;
	if (bOutOfTime || bTooFar)
	{
		FinishFlight();
	}
}

void ACrowActor::FinishFlight()
{
	State = ECrowState::Gone;
	Velocity = FVector::ZeroVector;
	SetActorTickEnabled(false);

	if (bDestroyAfterFlight)
	{
		Destroy();
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ACrowActor::ResetToIdle()
{
	GetWorldTimerManager().ClearTimer(StateTimer);

	AlarmInstigator = nullptr;
	Velocity = FVector::ZeroVector;
	FlightElapsed = 0.0f;

	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorTransform(IdleTransform);

	State = ECrowState::Idle;
	PlayIdle();
}
