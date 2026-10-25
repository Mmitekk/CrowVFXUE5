// Copyright Epic Games, Inc. All Rights Reserved.
// Sphere trigger that scares one or more crows when the player walks into it.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowTrigger.generated.h"

class USphereComponent;
class ACrowActor;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Crow Trigger"))
class UE52VFX_API ACrowTrigger : public AActor
{
	GENERATED_BODY()

public:
	ACrowTrigger();

	/** Overlap sphere of the trigger. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow Trigger|Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	/** Radius (cm) of the trigger. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger", meta = (ClampMin = "10", ClampMax = "20000"))
	float TriggerRadius = 650.0f;

	/** Extra delay (s) before the crows are scared. The crow's own TriggerDelay is added on top. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger", meta = (ClampMin = "0", ClampMax = "30"))
	float Delay = 0.0f;

	/** Only the player pawn fires the trigger. When off, any pawn does. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger")
	bool bOnlyPlayerPawn = true;

	/** Fire only once. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger")
	bool bTriggerOnce = true;

	/** Fill the Crow list automatically with the crows inside the radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger")
	bool bAutoFindCrows = true;

	/** Extra search radius (cm) used by Auto Find. 0 = use TriggerRadius * 1.5. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger", meta = (ClampMin = "0", ClampMax = "20000"))
	float AutoFindRadius = 0.0f;

	/** Crows scared by this trigger. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger")
	TArray<TObjectPtr<ACrowActor>> Crows;

	/** Draw the trigger sphere in the level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger")
	bool bDrawDebugRadius = true;

	/** Has the trigger already fired. */
	UPROPERTY(BlueprintReadOnly, Category = "Crow Trigger")
	bool bTriggered = false;

	/** Fire the trigger now, ignoring the overlap test. */
	UFUNCTION(BlueprintCallable, Category = "Crow Trigger")
	void Fire(AActor* InstigatorActor = nullptr);

	/** Forget the previous activation (for repeatable tests). */
	UFUNCTION(BlueprintCallable, Category = "Crow Trigger")
	void ResetTrigger();

	/** Called when the trigger fires with the actor that entered it. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Crow Trigger")
	void OnTriggerFired(AActor* InstigatorActor);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void CollectCrows();
	void FirePending();

	FTimerHandle DelayTimer;
	TWeakObjectPtr<AActor> PendingInstigator;
};
