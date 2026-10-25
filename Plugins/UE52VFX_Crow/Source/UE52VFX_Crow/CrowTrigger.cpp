#include "CrowTrigger.h"
#include "CrowActor.h"
#include "Kismet/KismetSystemLibrary.h"

ACrowTrigger::ACrowTrigger()
{
	// Create the trigger sphere
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	SetRootComponent(TriggerSphere);
	TriggerSphere->SetSphereRadius(ActivationRadius);
	TriggerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ACrowTrigger::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ACrowTrigger::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACrowTrigger::BeginPlay()
{
	Super::BeginPlay();
	// Register this class so it can be found across projects
	UClass* TriggerClass = ACrowTrigger::StaticClass();
	FName ClassName = TriggerClass->GetFName();
	UE_LOG(LogTemp, Log, TEXT("UE52VFX_Crow: Registered trigger class %s"), *ClassName.ToString());
}

// Called every frame
void ACrowTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACrowTrigger::TriggerCrow()
{
	if (bOneShot && CurrentOverlapCount > 0)
		return;

	// If we have a class specified and haven't spawned yet
	if (IsValid(CrowClass) && !IsValid(AssociatedCrow))
	{
		AssociatedCrow = GetWorld()->SpawnActor<ACrowActor>(CrowClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (IsValid(AssociatedCrow))
		{
			// Set parameters from this trigger
			if (IsValid(AssociatedCrow->CrowMesh))
			{
				AssociatedCrow->ActivationRadius = this->ActivationRadius;
				AssociatedCrow->TriggerDelay = this->TriggerDelay;
				AssociatedCrow->FlySpeed = 500.0f;
				AssociatedCrow->ClimbHeight = 200.0f;
				AssociatedCrow->MaxFlightTime = 10.0f;
				AssociatedCrow->DespawnDistance = 1000.0f;
			}
		}
	}
	else if (IsValid(AssociatedCrow))
	{
		// Trigger the existing crow to fly
		AssociatedCrow->TriggerFlee();
	}
}

void ACrowTrigger::ResetTrigger()
{
	CurrentOverlapCount = 0;
	bPlayerInTrigger = false;
	if (IsValid(AssociatedCrow))
	{
		AssociatedCrow->ResetToIdle();
	}
	AssociatedCrow = nullptr;
}

void ACrowTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int33 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		bPlayerInTrigger = true;
		CurrentOverlapCount++;

		if (bSpawnCrow && bOneShot && !IsValid(AssociatedCrow))
		{
			TriggerCrow();
		}
		else if (!bOneShot)
		{
			TriggerCrow();
		}
	}
}

void ACrowTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int33 OtherBodyIndex)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		bPlayerInTrigger = false;
		if (CurrentOverlapCount > 0)
		{
			CurrentOverlapCount--;
		}

		// If leaving and trigger is one-shot, reset
		if (bOneShot && CurrentOverlapCount == 0)
		{
			ResetTrigger();
		}
	}
}