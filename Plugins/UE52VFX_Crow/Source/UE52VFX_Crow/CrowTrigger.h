#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowTrigger.generated.h"

UCLASS(minimalapi, BlueprintSpawnableComponent)
class UE52VFX_CROW_API ACrowTrigger : public AActor
{
	GENERATED_BODY()

public:
	ACrowTrigger();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Sphere component for trigger area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* TriggerSphere;

	// Crow actor reference (optional - if you want this trigger to manage a crow)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	TSubclassOf<ACrowActor> CrowClass;

	// Activation radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0.01"))
	float ActivationRadius = 600.0f;

	// Delay before crow starts flying
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0.01"))
	float TriggerDelay = 1.0f;

	// Whether this trigger should spawn a crow
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	bool bSpawnCrow = true;

	// Whether this trigger should only fire once
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	bool bOneShot = true;

	// Current overlap count (for multi-shot triggers)
	UPROPERTY()
	int32 CurrentOverlapCount = 0;

	// Whether player is currently in trigger
	UPROPERTY()
	bool bPlayerInTrigger = false;

	// Internal crow reference
	UPROPERTY()
	ACrowActor* AssociatedCrow = nullptr;

	// Delegate for when crow is triggered
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void TriggerCrow();

	// Reset trigger state
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void ResetTrigger();

	// Check if crow is currently flying
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	bool IsCrowFlying() const { return AssociatedCrow && AssociatedCrow->IsFlying(); }
};