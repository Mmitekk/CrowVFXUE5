#include "SnowmanActor.h"
#include "CrowActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"

ASnowmanActor::ASnowmanActor()
{
	// Create the snowman mesh component
	SnowmanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnowmanMesh"));
	SetRootComponent(SnowmanMesh);

	// Create snow point (where crow lands)
	SnowPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SnowPoint"));
	SnowPoint->SetupAttachment(SnowmanMesh);

	// Create three radius spheres for different activation zones
	OuterSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphere"));
	OuterSphere->SetupAttachment(SnowmanMesh);
	OuterSphere->SetSphereRadius(OuterActivationRadius);
	OuterSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OuterSphere->OnComponentBeginOverlap.AddDynamic(this, &ASnowmanActor::OnOuterOverlap);

	MiddleSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MiddleSphere"));
	MiddleSphere->SetupAttachment(SnowmanMesh);
	MiddleSphere->SetSphereRadius(MiddleActivationRadius);
	MiddleSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	MiddleSphere->OnComponentBeginOverlap.AddDynamic(this, &ASnowmanActor::OnMiddleOverlap);

	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphere"));
	InnerSphere->SetupAttachment(SnowmanMesh);
	InnerSphere->SetSphereRadius(InnerActivationRadius);
	InnerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &ASnowmanActor::OnInnerOverlap);

	// Defaults
	CrowClass = ACrowActor::StaticClass();
	OuterActivationRadius = 800.0f;
	MiddleActivationRadius = 600.0f;
	InnerActivationRadius = 400.0f;
	CrowTriggerDelay = 1.5f;
	bFollowPlayer = true;

	// Initialize material references
	M_SnowBody = nullptr;
	M_Coal = nullptr;
	M_Carrot = nullptr;
	M_SnowGround = nullptr;
}

// Called when the game starts or when spawned
void ASnowmanActor::BeginPlay()
{
	Super::BeginPlay();

	// Create dynamic materials if base materials are set
	if (M_SnowBody)
	{
		DynamicMaterials.Add(SnowmanMesh->CreateDynamicMaterialInstance(0, M_SnowBody));
	}
	if (M_Coal)
	{
		// Coal could be a separate component or applied to mesh
	}
	if (M_Carrot)
	{
		// Carrot nose
	}
	if (M_SnowGround)
	{
		// Snow ground material
	}
}

// Called every frame
void ASnowmanActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Could add idle rotation or other behavior here
}

void ASnowmanActor::TriggerCrowFlight()
{
	if (!IsValid(CrowActor))
	{
		// Spawn the crow
		CrowActor = GetWorld()->SpawnActor<ACrowActor>(CrowClass, SnowPoint->GetComponentTransform());
		if (IsValid(CrowActor))
		{
			// Initialize crow parameters
			CrowActor->ActivationRadius = FMath::Max(1.0f, MiddleActivationRadius);
			CrowActor->TriggerDelay = CrowTriggerDelay;
			CrowActor->FlySpeed = 500.0f;
			CrowActor->ClimbHeight = 200.0f;
			CrowActor->MaxFlightTime = 10.0f;
			CrowActor->DespawnDistance = 1000.0f;

			// Play idle animation or start in idle state
			CrowActor->ResetToIdle();
		}
	}
	else
	{
		// Crow already exists, trigger it to fly
		CrowActor->TriggerFlee();
	}
}

void ASnowmanActor::ResetState()
{
	if (IsValid(CrowActor))
	{
		CrowActor->ResetToIdle();
	}
	else
	{
		// Reset internal state
		CrowActor = nullptr;
		// Reset material instances if needed
	}
}

void ASnowmanActor::OnOuterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int33 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		// Outer radius overlap - could trigger different behavior
		// For now, just trigger via middle sphere logic
	}
}

void ASnowmanActor::OnMiddleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int33 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		TriggerCrowFlight();
	}
}

void ASnowmanActor::OnInnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int33 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		// Inner radius - closest range, could trigger immediate reaction
	}
}