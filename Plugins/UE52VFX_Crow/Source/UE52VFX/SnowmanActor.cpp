// Copyright Epic Games, Inc. All Rights Reserved.

#include "SnowmanActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "CrowActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace SnowmanPrimitives
{
	static const float EngineSphereRadius = 50.0f;
	static const float EngineConeRadius = 50.0f;
	static const float EngineConeHeight = 100.0f;
	static const float EngineCylinderRadius = 50.0f;
	static const float EngineCylinderHeight = 100.0f;
}

ASnowmanActor::ASnowmanActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	auto MakePart = [this](const TCHAR* Name, UStaticMesh* MeshAsset) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Part->SetupAttachment(Root);
		if (MeshAsset != nullptr)
		{
			Part->SetStaticMesh(MeshAsset);
		}
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetGenerateOverlapEvents(false);
		Part->SetMobility(EComponentMobility::Movable);
		return Part;
	};

	BaseBall = MakePart(TEXT("BaseBall"), SphereMesh.Object);
	BodyBall = MakePart(TEXT("BodyBall"), SphereMesh.Object);
	HeadBall = MakePart(TEXT("HeadBall"), SphereMesh.Object);
	NoseCone = MakePart(TEXT("NoseCone"), ConeMesh.Object);
	LeftEye = MakePart(TEXT("LeftEye"), SphereMesh.Object);
	RightEye = MakePart(TEXT("RightEye"), SphereMesh.Object);
	LeftArm = MakePart(TEXT("LeftArm"), CylinderMesh.Object);
	RightArm = MakePart(TEXT("RightArm"), CylinderMesh.Object);

	PerchPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PerchPoint"));
	PerchPoint->SetupAttachment(Root);
}

void ASnowmanActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RebuildSnowman();
}

void ASnowmanActor::BeginPlay()
{
	Super::BeginPlay();

	if (!bSpawnCrowOnPerch)
	{
		return;
	}

	// Adopt a crow that was placed on the perch by hand first, otherwise spawn one.
	if (UWorld* World = GetWorld())
	{
		const FVector PerchLocation = GetPerchLocation();
		for (TActorIterator<ACrowActor> It(World); It; ++It)
		{
			if (FVector::Dist(It->GetActorLocation(), PerchLocation) <= 120.0f)
			{
				CrowOnPerch = *It;
				break;
			}
		}
	}

	if (CrowOnPerch == nullptr)
	{
		SpawnCrowOnPerch();
	}
}

void ASnowmanActor::RebuildSnowman()
{
	using namespace SnowmanPrimitives;

	const float BaseZ = BaseRadius * 0.85f;
	const float BodyZ = BaseZ + BaseRadius + BodyRadius * 0.45f;
	const float HeadZ = BodyZ + BodyRadius + HeadRadius * 0.45f;

	auto SetupSphere = [](UStaticMeshComponent* Part, float Radius, const FVector& Location, UMaterialInterface* Material)
	{
		if (Part == nullptr)
		{
			return;
		}
		Part->SetRelativeLocation(Location);
		Part->SetRelativeScale3D(FVector(Radius / EngineSphereRadius));
		Part->SetVisibility(true);
		if (Material != nullptr)
		{
			Part->SetMaterial(0, Material);
		}
	};

	SetupSphere(BaseBall, BaseRadius, FVector(0.0f, 0.0f, BaseZ), SnowMaterial);
	SetupSphere(BodyBall, BodyRadius, FVector(0.0f, 0.0f, BodyZ), SnowMaterial);
	SetupSphere(HeadBall, HeadRadius, FVector(0.0f, 0.0f, HeadZ), SnowMaterial);

	// Carrot nose, pointing forward (+X).
	if (NoseCone != nullptr)
	{
		NoseCone->SetVisibility(bShowNose);
		NoseCone->SetRelativeLocation(FVector(HeadRadius * 0.92f, 0.0f, HeadZ));
		NoseCone->SetRelativeRotation(FRotationMatrix::MakeFromZ(FVector::ForwardVector).Rotator());
		NoseCone->SetRelativeScale3D(FVector(
			NoseThickness / EngineConeRadius,
			NoseThickness / EngineConeRadius,
			NoseLength / EngineConeHeight));
		if (CarrotMaterial != nullptr)
		{
			NoseCone->SetMaterial(0, CarrotMaterial);
		}
	}

	// Coal eyes, placed on the head surface so they are visible from every side.
	const float EyeRadius = FMath::Max(HeadRadius * 0.10f, 1.5f);
	const float EyeSpacing = HeadRadius * 0.30f;
	const float EyeHeight = HeadRadius * 0.20f;
	const float EyeForward = FMath::Sqrt(FMath::Max(0.0f,
		HeadRadius * HeadRadius - EyeSpacing * EyeSpacing - EyeHeight * EyeHeight)) * 0.99f;
	const FVector EyeCenter(EyeForward, 0.0f, HeadZ + EyeHeight);
	if (LeftEye != nullptr)
	{
		LeftEye->SetVisibility(bShowEyes);
		LeftEye->SetRelativeLocation(EyeCenter + FVector(0.0f, -EyeSpacing, 0.0f));
		LeftEye->SetRelativeScale3D(FVector(EyeRadius / EngineSphereRadius));
		if (CoalMaterial != nullptr)
		{
			LeftEye->SetMaterial(0, CoalMaterial);
		}
	}
	if (RightEye != nullptr)
	{
		RightEye->SetVisibility(bShowEyes);
		RightEye->SetRelativeLocation(EyeCenter + FVector(0.0f, EyeSpacing, 0.0f));
		RightEye->SetRelativeScale3D(FVector(EyeRadius / EngineSphereRadius));
		if (CoalMaterial != nullptr)
		{
			RightEye->SetMaterial(0, CoalMaterial);
		}
	}

	// Stick arms.
	auto SetupArm = [&](UStaticMeshComponent* Part, float SideSign)
	{
		if (Part == nullptr)
		{
			return;
		}
		Part->SetVisibility(bShowArms);
		const FVector ArmDirection = FVector(0.0f, SideSign, FMath::Tan(FMath::DegreesToRadians(ArmRaiseAngle))).GetSafeNormal();
		Part->SetRelativeRotation(FRotationMatrix::MakeFromZ(ArmDirection).Rotator());
		Part->SetRelativeLocation(FVector(0.0f, SideSign * (BodyRadius * 0.80f), BodyZ + BodyRadius * 0.15f) + ArmDirection * (ArmLength * 0.5f));
		Part->SetRelativeScale3D(FVector(
			ArmThickness / EngineCylinderRadius,
			ArmThickness / EngineCylinderRadius,
			ArmLength / EngineCylinderHeight));
		if (CoalMaterial != nullptr)
		{
			Part->SetMaterial(0, CoalMaterial);
		}
	};
	SetupArm(LeftArm, -1.0f);
	SetupArm(RightArm, 1.0f);

	if (PerchPoint != nullptr)
	{
		PerchPoint->SetRelativeLocation(FVector(0.0f, 0.0f, HeadZ + HeadRadius * 0.9f) + PerchOffset);
	}
}

FVector ASnowmanActor::GetPerchLocation() const
{
	return (PerchPoint != nullptr) ? PerchPoint->GetComponentLocation() : GetActorLocation();
}

ACrowActor* ASnowmanActor::SpawnCrowOnPerch()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	UClass* ClassToSpawn = (CrowClass != nullptr) ? CrowClass.Get() : ACrowActor::StaticClass();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ACrowActor* Crow = World->SpawnActor<ACrowActor>(ClassToSpawn, GetPerchLocation(), GetActorRotation(), SpawnParams);
	CrowOnPerch = Crow;
	return Crow;
}
