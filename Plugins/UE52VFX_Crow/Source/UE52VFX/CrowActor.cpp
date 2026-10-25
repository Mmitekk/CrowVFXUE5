// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowActor.h"

#include "Animation/AnimSequence.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMesh.h"
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

	auto MakeAudio = [this](const TCHAR* Name) -> UAudioComponent*
	{
		UAudioComponent* Audio = CreateDefaultSubobject<UAudioComponent>(Name);
		Audio->SetupAttachment(CrowMesh);
		Audio->bAutoActivate = false;
		Audio->bAllowSpatialization = true;
		Audio->bOverrideAttenuation = false;
		return Audio;
	};

	CrowAudio = MakeAudio(TEXT("CrowAudio"));
	AlarmAudio = MakeAudio(TEXT("AlarmAudio"));
	FlightAudio = MakeAudio(TEXT("FlightAudio"));
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
	ScheduleNextIdleCaw();
}

float ACrowActor::GetComputedMeshYawOffset() const
{
	const USkeletalMesh* Mesh = nullptr;
	if (CrowSkeletalMesh != nullptr)
	{
		Mesh = CrowSkeletalMesh.Get();
	}
	else if (CrowMesh != nullptr)
	{
		Mesh = CrowMesh->GetSkeletalMeshAsset();
	}

	if (Mesh == nullptr)
	{
		return 0.0f;
	}

	const FReferenceSkeleton& Ref = Mesh->GetRefSkeleton();
	const int32 NumBones = Ref.GetNum();
	if (NumBones < 2)
	{
		return 0.0f;
	}

	// Component-space location of a bone in the reference pose.
	auto ComponentLocation = [&Ref](int32 Index) -> FVector
	{
		FTransform Accum = Ref.GetRefBonePose()[Index];
		for (int32 Parent = Ref.GetParentIndex(Index); Parent != INDEX_NONE; Parent = Ref.GetParentIndex(Parent))
		{
			Accum = Accum * Ref.GetRefBonePose()[Parent];
		}
		return Accum.GetLocation();
	};

	int32 RootIndex = Ref.FindBoneIndex(TEXT("root"));
	if (RootIndex == INDEX_NONE)
	{
		RootIndex = 0;
	}

	int32 HeadIndex = INDEX_NONE;
	for (int32 Index = 0; Index < NumBones; ++Index)
	{
		const FString BoneName = Ref.GetBoneName(Index).ToString();
		if (BoneName.Contains(TEXT("head"), ESearchCase::IgnoreCase))
		{
			HeadIndex = Index;
			break;
		}
	}

	if (HeadIndex == INDEX_NONE || HeadIndex == RootIndex)
	{
		return 0.0f;
	}

	FVector Forward = ComponentLocation(HeadIndex) - ComponentLocation(RootIndex);
	Forward.Z = 0.0f;
	if (Forward.SizeSquared() < 1.0f)
	{
		return 0.0f;
	}

	// Rotating the mesh by -Yaw(Forward) makes the computed forward direction point along +X.
	return -Forward.Rotation().Yaw;
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
	CrowMesh->SetRelativeScale3D(FVector(CrowScale));

	FRotator MeshRotation = MeshRotationOffset;
	if (bAutoAlignMeshForward)
	{
		MeshRotation.Yaw += GetComputedMeshYawOffset();
	}
	CrowMesh->SetRelativeRotation(MeshRotation);
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

// ---------------------------------------------------------------------------- sound

void ACrowActor::PlayRandomSound(UAudioComponent* Component, const TArray<TObjectPtr<USoundBase>>& Sounds,
	float Volume, USoundAttenuation* Attenuation)
{
	if (Component == nullptr || Sounds.Num() == 0)
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

	Component->Stop();
	Component->SetSound(Sound);
	Component->SetVolumeMultiplier(FMath::Max(Volume, 0.0f));
	Component->SetPitchMultiplier(FMath::Clamp(1.0f + FMath::FRandRange(-SoundPitchJitter, SoundPitchJitter), 0.1f, 3.0f));
	Component->bAllowSpatialization = bSpatializeSounds;

	// A raw sound wave has no attenuation of its own, so the component has to own it.
	Component->bOverrideAttenuation = (Attenuation != nullptr);
	Component->AttenuationSettings = Attenuation;

	Component->Play();
}

void ACrowActor::StopAllSounds()
{
	for (UAudioComponent* Component : {CrowAudio.Get(), AlarmAudio.Get(), FlightAudio.Get()})
	{
		if (Component != nullptr)
		{
			Component->Stop();
		}
	}
}

void ACrowActor::ScheduleNextIdleCaw()
{
	if (!bCawWhileIdle || IdleCawSounds.Num() == 0 || State != ECrowState::Idle || GetWorld() == nullptr)
	{
		return;
	}

	const float MinInterval = FMath::Max(IdleCawIntervalMin, 0.2f);
	const float MaxInterval = FMath::Max(IdleCawIntervalMax, MinInterval);
	GetWorldTimerManager().SetTimer(IdleCawTimer, this, &ACrowActor::HandleIdleCaw,
		FMath::FRandRange(MinInterval, MaxInterval), false);
}

void ACrowActor::HandleIdleCaw()
{
	if (State != ECrowState::Idle)
	{
		return;
	}

	PlayRandomSound(CrowAudio, IdleCawSounds, IdleCawVolume, IdleCawAttenuation);
	ScheduleNextIdleCaw();
}

// ---------------------------------------------------------------------------- blueprint events

void ACrowActor::OnCrowAlarmed_Implementation(AActor* InstigatorActor)
{
	// Empty by default: override it in the Blueprint and call the parent function if needed.
}

void ACrowActor::OnCrowTakeOff_Implementation()
{
}

void ACrowActor::OnCrowFlyAway_Implementation()
{
}

void ACrowActor::OnCrowFled_Implementation()
{
}

// ---------------------------------------------------------------------------- reaction

void ACrowActor::ScareInDirection(AActor* InstigatorActor, FVector Direction, float StartDelay, bool bPlayAlarmCaw)
{
	if (State != ECrowState::Idle)
	{
		return;
	}

	const FVector NormalizedDirection = Direction.GetSafeNormal();
	bHasFlightDirectionOverride = !NormalizedDirection.IsNearlyZero();
	FlightDirectionOverride = NormalizedDirection;
	bPlayAlarmSound = bPlayAlarmCaw;

	if (StartDelay <= 0.0f)
	{
		TriggerFlee(InstigatorActor);
		return;
	}

	if (GetWorld() == nullptr)
	{
		return;
	}

	FTimerDelegate DelayedScare = FTimerDelegate::CreateWeakLambda(this, [this, InstigatorActor]()
	{
		TriggerFlee(InstigatorActor);
	});
	GetWorldTimerManager().SetTimer(ScareDelayTimer, DelayedScare, StartDelay, false);
}

void ACrowActor::TriggerFlee(AActor* InstigatorActor)
{
	if (State != ECrowState::Idle)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(IdleCawTimer);
	GetWorldTimerManager().ClearTimer(ScareDelayTimer);

	AlarmInstigator = InstigatorActor;
	State = ECrowState::Alarmed;

	if (bPlayAlarmSound)
	{
		PlayRandomSound(AlarmAudio, AlarmedSounds, AlarmVolume, AlarmAttenuation);
	}
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

	PlayRandomSound(FlightAudio, TakeOffSounds, TakeOffVolume, TakeOffAttenuation);

	// Escape direction: an explicit one from Scare In Direction, otherwise away from
	// whoever scared the bird, with a bit of random spread.
	FVector Direction = GetActorForwardVector();
	if (bHasFlightDirectionOverride)
	{
		Direction = FlightDirectionOverride;
	}
	else if (bFlyAwayFromPlayer)
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

	// One shot: the explicit direction is used for this take-off only.
	bHasFlightDirectionOverride = false;

	// Face the escape direction right away so the bird never leaves sideways.
	FRotator TakeOffRotation = GetActorRotation();
	TakeOffRotation.Pitch = 0.0f;
	TakeOffRotation.Roll = 0.0f;
	TakeOffRotation.Yaw = FlightDirection.Rotation().Yaw;
	SetActorRotation(TakeOffRotation);

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
		FRotator DesiredRotation = Velocity.Rotation();
		DesiredRotation.Roll = 0.0f;
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaSeconds, 4.0f));
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
	if (State == ECrowState::Gone)
	{
		return;
	}

	State = ECrowState::Gone;
	Velocity = FVector::ZeroVector;
	SetActorTickEnabled(false);
	GetWorldTimerManager().ClearTimer(IdleCawTimer);
	StopAllSounds();

	// Fired exactly once, while the actor still exists, so the Blueprint can react to
	// "the bird has flown away" even when the actor is destroyed afterwards.
	bHasFled = true;
	OnCrowFled();

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
	GetWorldTimerManager().ClearTimer(IdleCawTimer);
	GetWorldTimerManager().ClearTimer(ScareDelayTimer);

	AlarmInstigator = nullptr;
	Velocity = FVector::ZeroVector;
	FlightElapsed = 0.0f;
	bHasFlightDirectionOverride = false;
	bPlayAlarmSound = true;

	StopAllSounds();
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorTransform(IdleTransform);
	ApplyMeshSettings();

	State = ECrowState::Idle;
	PlayIdle();
	ScheduleNextIdleCaw();
}
