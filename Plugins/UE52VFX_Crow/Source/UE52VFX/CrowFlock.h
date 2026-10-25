// Copyright Epic Games, Inc. All Rights Reserved.
// A flock of crows: spawns birds along a spline, then scatters them in their own
// directions with their own delay, with or without a caw. Independent from the snowman.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowFlock.generated.h"

class USceneComponent;
class USplineComponent;
class ACrowActor;

/** One bird of the flock. / Одна птица в стае. */
USTRUCT(BlueprintType)
struct FCrowFlockBird
{
	GENERATED_BODY()

	/** Flight direction in world degrees (0 = +X, 90 = +Y). / Направление полёта в мировых градусах (0 = +X, 90 = +Y). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird",
		meta = (ClampMin = "-360", ClampMax = "360",
			ToolTip = "EN: Direction this bird flies to, in world degrees around Z (0 = +X, 90 = +Y, 180 = -X, -90 = -Y). Random spread is added on top when Randomize Directions is on.\nRU: Направление, в котором улетает эта птица, в мировых градусах вокруг Z (0 = +X, 90 = +Y, 180 = -X, -90 = -Y). Случайный разброс прибавляется, если включено Randomize Directions."))
	float FlightYaw = 0.0f;

	/** Extra delay (s) before this bird takes off. / Доп. задержка (с) перед взлётом этой птицы. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird",
		meta = (ClampMin = "0", ClampMax = "60",
			ToolTip = "EN: Extra delay in seconds before this bird reacts, on top of Base Scare Delay. Gives the flock a staggered take-off.\nRU: Дополнительная задержка в секундах до реакции этой птицы, сверх Base Scare Delay. Даёт стае «волну» взлёта."))
	float Delay = 0.0f;

	/** Play a caw when this bird takes off. / Каркать при взлёте этой птицы. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird",
		meta = (ToolTip = "EN: On: this bird caws when scared. Off: it takes off silently.\nRU: Вкл: птица каркает при испуге. Выкл: взлетает молча."))
	bool bCaw = true;

	/** Scale override for this bird. 0 = use the flock scale. / Масштаб этой птицы. 0 = масштаб стаи. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird",
		meta = (ClampMin = "0", ClampMax = "10",
			ToolTip = "EN: Individual scale of this bird. 0 means the flock scale is used.\nRU: Индивидуальный масштаб птицы. 0 — используется масштаб стаи."))
	float Scale = 0.0f;
};

/**
 * A flock of crows dealt with as one actor: birds appear on the spline at Begin Play
 * and scatter on command, each one with its own direction, delay and caw flag.
 */
UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Crow Flock"))
class UE52VFX_API ACrowFlock : public AActor
{
	GENERATED_BODY()

public:
	ACrowFlock();

	// ------------------------------------------------------------------ components

	/** Actor root. / Корень актёра. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow Flock|Components",
		meta = (ToolTip = "EN: Root scene component.\nRU: Корневой компонент актёра."))
	TObjectPtr<USceneComponent> Root;

	/** Birds are placed along this spline at Begin Play. Move its points to shape the flock. / Птицы расставляются вдоль этого сплайна. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow Flock|Components",
		meta = (ToolTip = "EN: Spawn line. At Begin Play the birds are placed evenly along it (with optional jitter). Edit the points to shape the flock: a line, an arc, a circle.\nRU: Линия спавна. При старте птицы расставляются вдоль неё равномерно (с необязательным разбросом). Двигайте точки, чтобы задать линию, дугу или круг."))
	TObjectPtr<USplineComponent> SpawnSpline;

	// ------------------------------------------------------------------ flock

	/** Which bird class to spawn. / Какой класс птицы создавать. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock",
		meta = (ToolTip = "EN: Crow class spawned for every bird (usually BP_Crow). Each instance keeps its own audio components, so every bird is spatialized on its own.\nRU: Класс вороны для каждой птицы (обычно BP_Crow). У каждого экземпляра свои аудиокомпоненты, поэтому каждая птица звучит отдельно."))
	TSubclassOf<ACrowActor> CrowClass;

	/** How many birds the flock has. Changes the Birds array. / Сколько птиц в стае. Меняет размер массива Birds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock",
		meta = (ClampMin = "1", ClampMax = "64",
			ToolTip = "EN: Number of birds. The Bird Settings array is resized to match: one entry per bird with its own direction, delay, caw flag and scale.\nRU: Количество птиц. Массив Bird Settings подгоняется под это число: одна запись на птицу со своим направлением, задержкой, криком и масштабом."))
	int32 BirdCount = 8;

	/** Per-bird settings: direction, delay, caw, scale. / Настройки каждой птицы. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock",
		meta = (ToolTip = "EN: One entry per bird. Directions default to an even fan around the flock; change any value to make the flock fly apart unevenly.\nRU: Одна запись на птицу. Направления по умолчанию расходятся веером; меняйте значения, чтобы стая разлеталась неравномерно."))
	TArray<FCrowFlockBird> Birds;

	/** Scale of every bird, unless the bird entry overrides it. / Масштаб птиц, если у записи не задан свой. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock", meta = (ClampMin = "0.05", ClampMax = "10",
		ToolTip = "EN: Uniform scale applied to every spawned bird (bird entries can override it).\nRU: Общий масштаб созданных птиц (запись птицы может его переопределить)."))
	float CrowScale = 1.0f;

	/** Spawn the birds at Begin Play. / Создавать птиц при старте. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock",
		meta = (ToolTip = "EN: On: the birds are spawned when the game starts. Off: call Respawn Flock by hand.\nRU: Вкл: птицы создаются при старте игры. Выкл: вызовите Respawn Flock вручную."))
	bool bSpawnOnBeginPlay = true;

	/** Random offset (cm) along the spline for every bird. / Случайное смещение (см) вдоль сплайна для каждой птицы. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock", meta = (ClampMin = "0", ClampMax = "2000",
		ToolTip = "EN: Random offset in cm along the spline, so the birds do not sit in a mathematically even row.\nRU: Случайное смещение в сантиметрах вдоль сплайна, чтобы птицы не сидели строго ровно."))
	float SplinePlacementJitter = 35.0f;

	/** Silence the idle caw of every bird so the flock does not caw constantly. / Отключить карканье на присаде у всех птиц. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Flock",
		meta = (ToolTip = "EN: On: the flock birds do not caw while sitting, only at take-off. Off: each bird keeps its own idle caw interval.\nRU: Вкл: птицы не каркают, пока сидят, только при взлёте. Выкл: у каждой птицы свой интервал карканья на присаде."))
	bool bMuteIdleCaw = true;

	// ------------------------------------------------------------------ scare

	/** Scare the flock automatically when the player is that close. 0 = off. / Пугать стаю, когда игрок ближе этого. 0 = выкл. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare", meta = (ClampMin = "0", ClampMax = "20000",
		ToolTip = "EN: Radius in cm around the flock. When the player comes closer, the whole flock is scared. 0 disables the automatic scare.\nRU: Радиус в сантиметрах вокруг стаи. Когда игрок подходит ближе, вся стая взлетает. 0 отключает автоматический испуг."))
	float ScareRadius = 900.0f;

	/** Scare the flock right after spawning. / Пугать стаю сразу после спавна. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare",
		meta = (ToolTip = "EN: On: the flock takes off by itself after Start Delay, without waiting for the player. Off: it waits for Scare All or for the player.\nRU: Вкл: стая взлетает сама через Start Delay, не дожидаясь игрока. Выкл: ждёт вызова Scare All или игрока."))
	bool bScareOnBeginPlay = false;

	/** Delay (s) used by Scare On Begin Play. / Задержка (с) для Scare On Begin Play. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare", meta = (ClampMin = "0", ClampMax = "120",
		ToolTip = "EN: Delay in seconds before a Scare On Begin Play fire.\nRU: Задержка в секундах перед автоматическим испугом при старте."))
	float StartDelay = 3.0f;

	/** Extra delay (s) added to every bird. / Доп. задержка (с), общая для всех птиц. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare", meta = (ClampMin = "0", ClampMax = "60",
		ToolTip = "EN: Delay in seconds added to every bird on top of its own Delay.\nRU: Задержка в секундах, прибавляемая каждой птице к её собственной Delay."))
	float BaseScareDelay = 0.0f;

	/** Jitter the per-bird directions randomly. / Случайно разбрасывать направления. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare",
		meta = (ToolTip = "EN: On: a random spread is added to each bird direction, so repeated runs look different. Off: the exact directions from Bird Settings are used.\nRU: Вкл: к направлению каждой птицы прибавляется случайный разброс, чтобы повторы выглядели по-разному. Выкл: используются точные значения из Bird Settings."))
	bool bRandomizeDirections = true;

	/** Random spread (deg) for Randomize Directions. / Разброс (град) для Randomize Directions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare", meta = (ClampMin = "0", ClampMax = "180",
		ToolTip = "EN: Maximum random deviation in degrees added to a bird direction.\nRU: Максимальное случайное отклонение в градусах, прибавляемое к направлению птицы."))
	float DirectionSpread = 18.0f;

	/** Scare only the player pawn, not every pawn. / Реагировать только на игрока. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow Flock|Scare",
		meta = (ToolTip = "EN: On: only the player pawn scares the flock. Off: any pawn (animals, NPCs) does.\nRU: Вкл: стаю пугает только пешка игрока. Выкл: любая пешка (животные, NPC)."))
	bool bOnlyPlayerPawn = true;

	// ------------------------------------------------------------------ state

	/** Crows spawned by this flock. / Вороны, созданные стаей. */
	UPROPERTY(BlueprintReadOnly, Category = "Crow Flock|State",
		meta = (ToolTip = "EN: Birds currently owned by the flock. Read only.\nRU: Птицы, созданные стаей. Только чтение."))
	TArray<TObjectPtr<ACrowActor>> SpawnedCrows;

	/** Has the flock been scared already. / Стая уже взлетела. */
	UPROPERTY(BlueprintReadOnly, Category = "Crow Flock|State",
		meta = (ToolTip = "EN: True once the flock was scared. Reset Flock clears it.\nRU: True после испуга. Reset Flock сбрасывает."))
	bool bScared = false;

	// ------------------------------------------------------------------ api

	/** Scare every bird: own direction, own delay, caw or silent. / Испугать всех: своё направление, своя задержка, с криком или молча. */
	UFUNCTION(BlueprintCallable, Category = "Crow Flock",
		meta = (ToolTip = "EN: Sends every bird away along its own direction with its own delay. Does nothing when the flock is already scared.\nRU: Отправляет каждую птицу по её направлению с её задержкой. Ничего не делает, если стая уже взлетела."))
	void ScareAll(AActor* InstigatorActor = nullptr);

	/** Destroy the current birds and spawn a fresh flock. / Удалить птиц и создать стаю заново. */
	UFUNCTION(BlueprintCallable, Category = "Crow Flock",
		meta = (ToolTip = "EN: Destroys the current birds and spawns a fresh flock on the spline. Handy for testing the scene again.\nRU: Удаляет текущих птиц и создаёт стаю заново на сплайне. Удобно, чтобы проиграть сцену повторно."))
	void RespawnFlock();

	/** Spawn the birds now, without scaring them. / Создать птиц сейчас, не пугая. */
	UFUNCTION(BlueprintCallable, Category = "Crow Flock",
		meta = (ToolTip = "EN: Spawns the birds on the spline right away. Does nothing when the birds already exist.\nRU: Создаёт птиц на сплайне сразу. Ничего не делает, если птицы уже есть."))
	void SpawnBirds();

	/** How many birds are alive right now. / Сколько птиц сейчас живо. */
	UFUNCTION(BlueprintPure, Category = "Crow Flock",
		meta = (ToolTip = "EN: Number of spawned birds that still exist.\nRU: Количество созданных птиц, которые ещё существуют."))
	int32 GetSpawnedBirdCount() const;

	/** Called after the birds are spawned. / Вызывается после создания птиц. */
	UFUNCTION(BlueprintNativeEvent, Category = "Crow Flock",
		meta = (ToolTip = "EN: Fired after the birds are placed on the spline. Override it in the Blueprint for extra setup.\nRU: Срабатывает после расстановки птиц на сплайне. Переопределяйте в блюпринте для доп. настройки."))
	void OnFlockSpawned();

	/** Called when the flock is scared. / Вызывается, когда стая взлетает. */
	UFUNCTION(BlueprintNativeEvent, Category = "Crow Flock",
		meta = (ToolTip = "EN: Fired when the flock is scared, before the birds take off. Override it in the Blueprint to hang your own logic.\nRU: Срабатывает при испуге стаи, до взлёта птиц. Переопределяйте в блюпринте, чтобы повесить свою логику."))
	void OnFlockScared(AActor* InstigatorActor);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ApplyBirdDefaults(int32 NewCount);
	APawn* FindScarePawn() const;

	FTimerHandle AutoScareTimer;
};
