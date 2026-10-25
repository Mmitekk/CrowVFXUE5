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

	/** Overlap sphere of the trigger. / Сфера-триггер. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow Trigger|Components",
		meta = (ToolTip = "EN: The overlap sphere. Its radius comes from Trigger Radius.\nRU: Сфера перекрытия. Радиус берётся из Trigger Radius."))
	TObjectPtr<USphereComponent> TriggerSphere;

	/** Radius (cm) of the trigger. / Радиус триггера (см). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger", meta = (ClampMin = "10", ClampMax = "20000",
		ToolTip = "EN: Radius of the trigger sphere in cm.\nRU: Радиус сферы триггера в сантиметрах."))
	float TriggerRadius = 650.0f;

	/** Extra delay (s) before the crows are scared. The crow's own TriggerDelay is added on top. / Доп. задержка (с). TriggerDelay вороны добавляется сверху. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger", meta = (ClampMin = "0", ClampMax = "30",
		ToolTip = "EN: Extra delay in seconds before the crows are scared. The crow's own Trigger Delay is added on top of it.\nRU: Дополнительная задержка в секундах до испуга ворон. Собственный Trigger Delay вороны прибавляется сверху."))
	float Delay = 0.0f;

	/** Only the player pawn fires the trigger. When off, any pawn does. / Срабатывать только на игрока. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger",
		meta = (ToolTip = "EN: On: only the player pawn fires the trigger. Off: any pawn (animals, NPCs) does.\nRU: Вкл: срабатывает только на пешку игрока. Выкл: на любую пешку (животные, NPC)."))
	bool bOnlyPlayerPawn = true;

	/** Fire only once. / Срабатывать только один раз. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger",
		meta = (ToolTip = "EN: On: the trigger fires once. Off: it fires every time a pawn walks in.\nRU: Вкл: триггер срабатывает один раз. Выкл: при каждом входе пешки."))
	bool bTriggerOnce = true;

	/** Fill the Crow list automatically with the crows inside the radius. / Автоматически собирать ворон в радиусе. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger",
		meta = (ToolTip = "EN: On: the Crow list is filled automatically with every crow inside the search radius.\nRU: Вкл: список Crows автоматически заполняется всеми воронами в радиусе поиска."))
	bool bAutoFindCrows = true;

	/** Extra search radius (cm) used by Auto Find. 0 = use TriggerRadius * 1.5. / Радиус поиска ворон (см). 0 = TriggerRadius * 1.5. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger", meta = (ClampMin = "0", ClampMax = "20000",
		ToolTip = "EN: Radius in cm used by Auto Find. 0 = Trigger Radius multiplied by 1.5.\nRU: Радиус в сантиметрах для авто-поиска ворон. 0 = Trigger Radius умножить на 1.5."))
	float AutoFindRadius = 0.0f;

	/** Crows scared by this trigger. / Вороны, которых пугает этот триггер. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger",
		meta = (ToolTip = "EN: Crows scared by this trigger. Filled by hand, or automatically when Auto Find Crows is on.\nRU: Вороны, которых пугает триггер. Заполняется вручную или автоматически при включённом Auto Find Crows."))
	TArray<TObjectPtr<ACrowActor>> Crows;

	/** Draw the trigger sphere in the level. / Рисовать сферу триггера на уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Trigger",
		meta = (ToolTip = "EN: Draws the trigger sphere in the level viewport. Debug only, it is not drawn in the game.\nRU: Рисует сферу триггера во вьюпорте. Только для отладки, в игре не рисуется."))
	bool bDrawDebugRadius = true;

	/** Has the trigger already fired. / Триггер уже сработал. */
	UPROPERTY(BlueprintReadOnly, Category = "Crow Trigger",
		meta = (ToolTip = "EN: True once the trigger fired. Read only.\nRU: True после срабатывания триггера. Только чтение."))
	bool bTriggered = false;

	/** Fire the trigger now, ignoring the overlap test. / Сработать прямо сейчас, минуя перекрытие. */
	UFUNCTION(BlueprintCallable, Category = "Crow Trigger",
		meta = (ToolTip = "EN: Fires the trigger immediately, without waiting for an overlap.\nRU: Запускает триггер сразу, не дожидаясь перекрытия."))
	void Fire(AActor* InstigatorActor = nullptr);

	/** Forget the previous activation (for repeatable tests). / Сбросить срабатывание (для повторных тестов). */
	UFUNCTION(BlueprintCallable, Category = "Crow Trigger",
		meta = (ToolTip = "EN: Clears the fired flag and the pending delay, so the trigger can fire again.\nRU: Сбрасывает флаг срабатывания и отложенный вызов, чтобы триггер мог сработать снова."))
	void ResetTrigger();

	/** Called when the trigger fires with the actor that entered it. / Вызывается при срабатывании. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Crow Trigger",
		meta = (ToolTip = "EN: Blueprint event fired when the trigger goes off, with the actor that entered it.\nRU: Событие блюпринта при срабатывании, передаёт вошедшего актёра."))
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
