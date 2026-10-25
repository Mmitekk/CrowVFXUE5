// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowTrigger.h"

#include "Components/SphereComponent.h"
#include "CrowActor.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

ACrowTrigger::ACrowTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	SetRootComponent(TriggerSphere);
	TriggerSphere->InitSphereRadius(TriggerRadius);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerSphere->SetGenerateOverlapEvents(true);
	TriggerSphere->SetHiddenInGame(true);
}

void ACrowTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (TriggerSphere != nullptr)
	{
		TriggerSphere->SetSphereRadius(TriggerRadius, false);
	}
}

void ACrowTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerSphere != nullptr)
	{
		TriggerSphere->SetSphereRadius(TriggerRadius, true);
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ACrowTrigger::HandleBeginOverlap);

		// The player may already be standing inside the trigger when the game starts.
		TArray<AActor*> Overlapping;
		TriggerSphere->GetOverlappingActors(Overlapping, APawn::StaticClass());
		for (AActor* Actor : Overlapping)
		{
			HandleBeginOverlap(TriggerSphere, Actor, nullptr, 0, false, FHitResult());
			if (bTriggered && bTriggerOnce)
			{
				break;
			}
		}
	}

	CollectCrows();
}

void ACrowTrigger::CollectCrows()
{
	if (!bAutoFindCrows || GetWorld() == nullptr)
	{
		return;
	}

	const float SearchRadius = (AutoFindRadius > 0.0f) ? AutoFindRadius : TriggerRadius * 1.5f;
	const FVector Center = GetActorLocation();

	for (TActorIterator<ACrowActor> It(GetWorld()); It; ++It)
	{
		if (FVector::Dist(It->GetActorLocation(), Center) <= SearchRadius && !Crows.Contains(*It))
		{
			Crows.Add(*It);
		}
	}

	Crows.RemoveAll([](const TObjectPtr<ACrowActor>& Crow) { return Crow == nullptr; });
}

void ACrowTrigger::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDrawDebugRadius && GetWorld() != nullptr)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), TriggerRadius, 32, bTriggered ? FColor(255, 190, 40) : FColor(60, 160, 255), false, 0.0f, 0, 1.5f);
	}
}

void ACrowTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}
	if (bTriggerOnce && bTriggered)
	{
		return;
	}

	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn == nullptr)
	{
		return;
	}
	if (bOnlyPlayerPawn && !Pawn->IsPlayerControlled())
	{
		return;
	}

	if (Delay > 0.0f)
	{
		if (!GetWorldTimerManager().IsTimerActive(DelayTimer))
		{
			PendingInstigator = OtherActor;
			GetWorldTimerManager().SetTimer(DelayTimer, this, &ACrowTrigger::FirePending, Delay, false);
		}
	}
	else
	{
		Fire(OtherActor);
	}
}

void ACrowTrigger::FirePending()
{
	Fire(PendingInstigator.Get());
	PendingInstigator = nullptr;
}

void ACrowTrigger::Fire(AActor* InstigatorActor)
{
	if (bTriggerOnce && bTriggered)
	{
		return;
	}

	bTriggered = true;
	if (bAutoFindCrows)
	{
		CollectCrows();
	}

	OnTriggerFired(InstigatorActor);

	for (const TObjectPtr<ACrowActor>& Crow : Crows)
	{
		if (Crow != nullptr)
		{
			Crow->TriggerFlee(InstigatorActor);
		}
	}
}

void ACrowTrigger::ResetTrigger()
{
	GetWorldTimerManager().ClearTimer(DelayTimer);
	PendingInstigator = nullptr;
	bTriggered = false;
}
