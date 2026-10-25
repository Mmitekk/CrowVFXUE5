#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowmanActor.generated.h"

UCLASS(minimalapi, BlueprintSpawnableComponent)
class UE52VFX_CROW_API ASnowmanActor : public AActor
{
	GENERATED_BODY()

public:
	ASnowmanActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Called when player overlaps the trigger sphere
	UFUNCTION(BlueprintCallable, Category = "Snowman")
	void TriggerCrowFlight();

	// Reset the snowman state
	UFUNCTION(BlueprintCallable, Category = "Snowman")
	void ResetState();

	// Get the current crow actor
	UFUNCTION(BlueprintCallable, Category = "Snowman")
	ACrowActor* GetCrowActor() const { return CrowActor; }

	// Snowman mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* SnowmanMesh;

	// Materials for snowman parts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* M_SnowBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* M_Coal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* M_Carrot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* M_SnowGround;

	// Crow class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Settings")
	TSubclassOf<ACrowActor> CrowClass;

	// Radius for crow activation (three zones)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Settings", meta = (ClampMin = "0.01"))
	float OuterActivationRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Settings", meta = (ClampMin = "0.01"))
	float MiddleActivationRadius = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Settings", meta = (ClampMin = "0.01"))
	float InnerActivationRadius = 400.0f;

	// Delay before crow flies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Settings", meta = (ClampMin = "0.01"))
	float CrowTriggerDelay = 1.5f;

	// Whether crow should follow player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Settings")
	bool bFollowPlayer = true;

	// Snow point (where crow lands)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SnowPoint;

	// Material instances (created at runtime)
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	// Internal crow reference
	UPROPERTY()
	ACrowActor* CrowActor = nullptr;

	// Sphere components for different activation radii
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* OuterSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* MiddleSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* InnerSphere;
};