// Copyright Epic Games, Inc. All Rights Reserved.
// Perched crow: sits on a snowman, caws when the player gets close and flies away.
// Everything gameplay-facing is exposed to Blueprint / details panel.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowActor.generated.h"

class USkeletalMeshComponent;
class UAudioComponent;
class USkeletalMesh;
class UMaterialInterface;
class UAnimSequence;
class USoundBase;
class USoundAttenuation;

/** What the crow is doing right now. */
UENUM(BlueprintType)
enum class ECrowState : uint8
{
	/** Sitting on its perch. */
	Idle,
	/** Player came close, cawing, waiting for TriggerDelay to take off. */
	Alarmed,
	/** Take-off animation is playing, the bird is already moving up. */
	TakingOff,
	/** Flapping away. */
	Flying,
	/** Flight is over (hidden or destroyed). */
	Gone
};

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Crow"))
class UE52VFX_API ACrowActor : public AActor
{
	GENERATED_BODY()

public:
	ACrowActor();

	// ------------------------------------------------------------------ components

	/** Plain root, keeps the actor transform independent from the mesh offset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components")
	TObjectPtr<USceneComponent> CrowRoot;

	/** Skeletal mesh of the crow. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components")
	TObjectPtr<USkeletalMeshComponent> CrowMesh;

	/** Handles the caw / wing flap sounds, spatialized through SoundAttenuation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components")
	TObjectPtr<UAudioComponent> CrowAudio;

	// ------------------------------------------------------------------ mesh

	/** Skeletal mesh to use. Leave empty to keep whatever the mesh component already has. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh")
	TObjectPtr<USkeletalMesh> CrowSkeletalMesh;

	/** Optional material override for slot 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh")
	TObjectPtr<UMaterialInterface> CrowMaterialOverride;

	/** Offset of the mesh inside the actor (use it to put the feet exactly on the perch). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh")
	FVector MeshRelativeLocation = FVector::ZeroVector;

	/** Rotation of the mesh inside the actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh")
	FRotator MeshRelativeRotation = FRotator::ZeroRotator;

	/** Uniform scale of the crow. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh", meta = (ClampMin = "0.05", ClampMax = "10"))
	float CrowScale = 1.0f;

	// ------------------------------------------------------------------ animation

	/** Looping animation played while the crow is sitting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation")
	TObjectPtr<UAnimSequence> IdleAnimation;

	/** One-shot animation played at take-off. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation")
	TObjectPtr<UAnimSequence> TakeOffAnimation;

	/** Looping animation played during the flight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation")
	TObjectPtr<UAnimSequence> FlyAnimation;

	/** Play rate of the idle animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation", meta = (ClampMin = "0.05", ClampMax = "4"))
	float IdlePlayRate = 1.0f;

	/** Play rate of the flight animation (wing beat speed). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation", meta = (ClampMin = "0.05", ClampMax = "4"))
	float FlyPlayRate = 1.0f;

	/** Time scale of the take-off animation. Values > 1 make the jump quicker. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation", meta = (ClampMin = "0.1", ClampMax = "4"))
	float TakeOffAnimationTimeScale = 1.0f;

	// ------------------------------------------------------------------ sound

	/** Sound cues played the moment the crow gets alarmed (caw). One is picked at random. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound")
	TArray<TObjectPtr<USoundBase>> AlarmedSounds;

	/** Sound cues played while leaving the perch (wing flaps). One is picked at random. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound")
	TArray<TObjectPtr<USoundBase>> TakeOffSounds;

	/** Attenuation asset — controls how loud the bird is and how far its voice carries. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound")
	TObjectPtr<USoundAttenuation> SoundAttenuation;

	/** Master volume of the alarm sounds (multiplied with the attenuation). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound", meta = (ClampMin = "0", ClampMax = "4"))
	float AlarmedSoundVolume = 1.0f;

	/** Master volume of the take-off (flap) sounds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound", meta = (ClampMin = "0", ClampMax = "4"))
	float TakeOffSoundVolume = 1.0f;

	/** Spatialize the sounds (false = flat 2D sound, useful for testing). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound")
	bool bSpatializeSounds = true;

	// ------------------------------------------------------------------ reaction

	/** The crow watches its own distance to the player. Turn off to drive it only from a trigger. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction")
	bool bUseProximityTrigger = true;

	/** Distance (cm) at which the player scares the bird. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "10000"))
	float ActivationRadius = 550.0f;

	/** Delay (s) between the alarm and the jump; the crow caws during this time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "10"))
	float TriggerDelay = 0.35f;

	/** React to the player pawn only. When off, any pawn scares the crow. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction")
	bool bOnlyReactToPlayer = true;

	/** Slowly turn the head/body towards the player before he gets too close. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction")
	bool bLookAtPlayer = true;

	/** Distance (cm) within which the crow starts tracking the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "10000"))
	float LookAtRadius = 1600.0f;

	/** Turn speed while tracking the player (deg/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "720"))
	float LookAtTurnSpeed = 70.0f;

	/** Draw the activation radius in the level (green = idle, red = triggered). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction")
	bool bDrawActivationRadius = false;

	// ------------------------------------------------------------------ flight

	/** Fly away from the player instead of straight ahead. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight")
	bool bFlyAwayFromPlayer = true;

	/** Random spread (deg) added to the escape direction so repeated runs look different. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "180"))
	float DirectionRandomSpreadDeg = 20.0f;

	/** Initial upward speed (cm/s) of the jump. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "3000"))
	float TakeOffVerticalSpeed = 420.0f;

	/** Cruise speed (cm/s) of the flight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "5000"))
	float FlySpeed = 900.0f;

	/** Acceleration (cm/s^2) towards the cruising speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "10", ClampMax = "10000"))
	float FlyAcceleration = 900.0f;

	/** How high (cm) the bird climbs before levelling out. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "10000"))
	float ClimbHeight = 800.0f;

	/** Vertical speed (cm/s) while climbing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "2000"))
	float ClimbSpeed = 320.0f;

	/** How long (s) the bird stays in the air before it is removed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0.5", ClampMax = "120"))
	float MaxFlightTime = 12.0f;

	/** Distance (cm) from the perch after which the bird is removed. 0 = ignore. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "100000"))
	float DespawnDistance = 6000.0f;

	/** Destroy the actor once the flight is over. Off = just hide it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight")
	bool bDestroyAfterFlight = true;

	/** Turn off collision on take-off so the bird never pushes the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight")
	bool bDisableCollisionOnTakeOff = true;

	// ------------------------------------------------------------------ state

	/** Current state of the bird. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Crow|State")
	ECrowState State = ECrowState::Idle;

	// ------------------------------------------------------------------ api

	/** Scare the crow: it caws, waits TriggerDelay and flies away. Safe to call many times. */
	UFUNCTION(BlueprintCallable, Category = "Crow")
	void TriggerFlee(AActor* InstigatorActor = nullptr);

	/** Put the bird back on its perch (useful for testing and for respawning). */
	UFUNCTION(BlueprintCallable, Category = "Crow")
	void ResetToIdle();

	/** Current state. */
	UFUNCTION(BlueprintPure, Category = "Crow")
	ECrowState GetCrowState() const { return State; }

	/** Distance to the pawn the crow reacts to, or -1 when there is none. */
	UFUNCTION(BlueprintPure, Category = "Crow")
	float GetDistanceToActivationTarget() const;

	/** Called when the crow notices the player (before the take-off). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Crow")
	void OnCrowAlarmed(AActor* InstigatorActor);

	/** Called when the bird leaves the perch. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Crow")
	void OnCrowTakeOff();

	/** Called when the take-off animation switches to the flight loop. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Crow")
	void OnCrowFlyAway();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ApplyMeshSettings();
	void PlayAnimationSequence(UAnimSequence* Anim, bool bLoop, float Rate);
	void PlayIdle();
	void EnterTakeOff();
	void EnterFlying();
	void FinishFlight();
	void PlayRandomSound(const TArray<TObjectPtr<USoundBase>>& Sounds, float Volume);
	APawn* FindReactionPawn() const;
	void UpdateIdle(float DeltaSeconds);
	void UpdateFlying(float DeltaSeconds);

	FTimerHandle StateTimer;

	/** Transform the crow was placed at, used by ResetToIdle. */
	FTransform IdleTransform;

	/** Who scared the bird (used for the escape direction). */
	TWeakObjectPtr<AActor> AlarmInstigator;

	FVector FlightDirection = FVector::ForwardVector;
	FVector Velocity = FVector::ZeroVector;
	FVector FlightStartLocation = FVector::ZeroVector;
	float FlightElapsed = 0.0f;
};
