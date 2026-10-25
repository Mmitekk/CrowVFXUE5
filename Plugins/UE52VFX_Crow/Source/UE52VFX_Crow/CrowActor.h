#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowActor.generated.h"

UCLASS()
class UE52VFX_CROW_API ACrowActor : public AActor
{
	GENERATED_BODY()

public:
	ACrowActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Skeletal mesh component for the crow */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* CrowMesh;

	/** Audio component for crow sounds */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* AudioComponent;

	/** Sound cue to play when crow takes off */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* TakeOffSoundCue;

	/** Sound cue for wing flap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* WingFlapSoundCue;

	/** Sound cue for crow call */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* CallSoundCue;

	/** Attenuation sound setting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundAttenuation* CrowAttenuation;

	/** Radius within which crow reacts to player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (ClampMin = "0.01"))
	float ActivationRadius = 600.0f;

	/** Delay before crow starts flying after activation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (ClampMin = "0.01"))
	float TriggerDelay = 1.0f;

	/** How fast crow flies upward */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (ClampMin = "0.01"))
	float ClimbHeight = 200.0f;

	/** Maximum flight time before despawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (ClampMin = "0.01"))
	float MaxFlightTime = 10.0f;

	/** Despawn distance from original position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (ClampMin = "0.01"))
	float DespawnDistance = 1000.0f;

	/** Flight speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (ClampMin = "0.01"))
	float FlySpeed = 500.0f;

	/** Whether to destroy actor after flight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	bool bDestroyAfterFlight = true;

	// Internal state
	UFUNCTION(BlueprintCallable, Category = "Crow")
	void TriggerFlee();

	UFUNCTION(BlueprintCallable, Category = "Crow")
	void ResetToIdle();

	UFUNCTION(BlueprintCallable, Category = "Crow")
	bool IsFlying() const { return CurrentState == ECrowState::Flying; }

protected:
	enum class ECrowState : uint8
	{
		Idle,
		Alarmed,
		TakingOff,
		Flying,
		Landing
	};

	UPROPERTY()
	ECrowState CurrentState = ECrowState::Idle;

	UPROPERTY()
	float Timer = 0.0f;

	UPROPERTY()
	class APawn* TargetPawn = nullptr;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int33 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UpdateCrowFlight(float DeltaTime);
};