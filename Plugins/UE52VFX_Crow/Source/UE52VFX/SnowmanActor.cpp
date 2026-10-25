// Copyright Epic Games, Inc. All Rights Reserved.

#include "SnowmanActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CrowActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Materials/MaterialInterface.h"

ASnowmanActor::ASnowmanActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SnowmanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnowmanMesh"));
	SnowmanMesh->SetupAttachment(Root);
	SnowmanMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SnowmanMesh->SetCollisionObjectType(ECC_WorldStatic);
	SnowmanMesh->SetMobility(EComponentMobility::Movable);

	PerchPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PerchPoint"));
	PerchPoint->SetupAttachment(Root);
	PerchPoint->SetMobility(EComponentMobility::Movable);
	PerchPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
}

void ASnowmanActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Only the mesh is driven by the exposed values. The perch keeps whatever transform
	// the user gave it, so it can be dragged around freely.
	ApplyMeshSettings();
}

void ASnowmanActor::BeginPlay()
{
	Super::BeginPlay();

	ApplyMeshSettings();

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

void ASnowmanActor::ApplyMeshSettings()
{
	if (SnowmanMesh == nullptr)
	{
		return;
	}

	SnowmanMesh->SetStaticMesh(SnowmanStaticMesh);
	if (SnowmanMaterial != nullptr)
	{
		SnowmanMesh->SetMaterial(0, SnowmanMaterial);
	}

	SnowmanMesh->SetRelativeLocation(MeshLocationOffset);
	SnowmanMesh->SetRelativeRotation(MeshRotationOffset);
	SnowmanMesh->SetRelativeScale3D(FVector(SnowmanScale));
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

	const FRotator SpawnRotation = bAlignCrowToPerch && PerchPoint != nullptr
		? PerchPoint->GetComponentRotation()
		: GetActorRotation();

	ACrowActor* Crow = World->SpawnActor<ACrowActor>(ClassToSpawn, GetPerchLocation(), SpawnRotation, SpawnParams);
	CrowOnPerch = Crow;
	return Crow;
}
