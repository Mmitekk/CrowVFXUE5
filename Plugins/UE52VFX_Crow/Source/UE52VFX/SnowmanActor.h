// Copyright Epic Games, Inc. All Rights Reserved.
// Snowman assembled from primitive meshes; it also owns the perch where the crow sits.
// All proportions and materials are exposed to Blueprint / details panel.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowmanActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class USceneComponent;
class UMaterialInterface;
class ACrowActor;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Snowman"))
class UE52VFX_API ASnowmanActor : public AActor
{
	GENERATED_BODY()

public:
	ASnowmanActor();

	// ------------------------------------------------------------------ components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> BaseBall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> BodyBall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> HeadBall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> NoseCone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> LeftEye;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> RightEye;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> LeftArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<UStaticMeshComponent> RightArm;

	/** Where the crow sits. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components")
	TObjectPtr<USceneComponent> PerchPoint;

	// ------------------------------------------------------------------ look

	/** Radius (cm) of the bottom snowball. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "5", ClampMax = "200"))
	float BaseRadius = 55.0f;

	/** Radius (cm) of the middle snowball. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "5", ClampMax = "200"))
	float BodyRadius = 40.0f;

	/** Radius (cm) of the head snowball. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "5", ClampMax = "200"))
	float HeadRadius = 28.0f;

	/** Snow material. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look")
	TObjectPtr<UMaterialInterface> SnowMaterial;

	/** Material of the nose. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look")
	TObjectPtr<UMaterialInterface> CarrotMaterial;

	/** Material of the eyes and buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look")
	TObjectPtr<UMaterialInterface> CoalMaterial;

	/** Show the carrot nose. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look")
	bool bShowNose = true;

	/** Show the coal eyes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look")
	bool bShowEyes = true;

	/** Show the stick arms. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look")
	bool bShowArms = true;

	/** Nose length (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "3", ClampMax = "120"))
	float NoseLength = 30.0f;

	/** Nose thickness (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "1", ClampMax = "30"))
	float NoseThickness = 7.0f;

	/** Arm length (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "5", ClampMax = "200"))
	float ArmLength = 55.0f;

	/** Arm thickness (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "0.5", ClampMax = "15"))
	float ArmThickness = 3.0f;

	/** How high (deg above horizontal) the arms point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Look", meta = (ClampMin = "-60", ClampMax = "60"))
	float ArmRaiseAngle = 20.0f;

	// ------------------------------------------------------------------ crow

	/** Which crow class to place on the perch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Crow")
	TSubclassOf<ACrowActor> CrowClass;

	/** Spawn the crow automatically on Begin Play. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Crow")
	bool bSpawnCrowOnPerch = true;

	/** The crow currently sitting on the snowman (spawned or placed by hand). */
	UPROPERTY(BlueprintReadOnly, Category = "Snowman|Crow")
	TObjectPtr<ACrowActor> CrowOnPerch;

	/** Local offset of the perch relative to the top of the head. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Crow")
	FVector PerchOffset = FVector(-6.0f, 0.0f, 0.0f);

	/** World location of the perch. */
	UFUNCTION(BlueprintPure, Category = "Snowman")
	FVector GetPerchLocation() const;

	/** Spawn (or move) the crow on the perch. */
	UFUNCTION(BlueprintCallable, Category = "Snowman")
	ACrowActor* SpawnCrowOnPerch();

	/** Rebuild the parts from the current settings. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Snowman")
	void RebuildSnowman();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
};
