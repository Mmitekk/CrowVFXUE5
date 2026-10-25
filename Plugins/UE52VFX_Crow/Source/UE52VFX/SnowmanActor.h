// Copyright Epic Games, Inc. All Rights Reserved.
// Snowman made of one static mesh; it also owns the perch where the crow sits.
// The perch is a normal editable component: move it wherever the bird has to sit.

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

	/** Root of the actor. / Корень актёра. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components",
		meta = (ToolTip = "EN: Root component. Do not edit its transform, move the actor instead.\nRU: Корневой компонент. Не меняйте его трансформ, перемещайте самого актёра."))
	TObjectPtr<USceneComponent> Root;

	/** The single static mesh that makes the whole snowman. / Единственная статическая сетка, из которой состоит весь снеговик. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snowman|Components",
		meta = (ToolTip = "EN: The only visual component of the snowman. Assign the mesh in Snowman Mesh.\nRU: Единственный визуальный компонент снеговика. Сетка назначается в Snowman Mesh."))
	TObjectPtr<UStaticMeshComponent> SnowmanMesh;

	/** Where the crow sits. Move it in the Blueprint or in the details panel of a placed actor. / Где сидит ворона. Перемещайте в блюпринте или в панели Details у экземпляра. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Snowman|Components",
		meta = (ToolTip = "EN: The perch point. Its transform is never overwritten by the code, so you can drag it anywhere (Blueprint viewport, details panel, or the components tree of a placed actor).\nRU: Точка присады. Код никогда не перезаписывает её трансформ, поэтому её можно двигать куда угодно (вьюпорт блюпринта, панель Details или дерево компонентов у экземпляра на уровне)."))
	TObjectPtr<USceneComponent> PerchPoint;

	// ------------------------------------------------------------------ mesh

	/** Static mesh of the whole snowman. / Статическая сетка всего снеговика. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Mesh",
		meta = (ToolTip = "EN: Static mesh that is drawn as the snowman. Empty = nothing is visible.\nRU: Статическая сетка, которая отображается как снеговик. Пусто = ничего не видно."))
	TObjectPtr<UStaticMesh> SnowmanStaticMesh;

	/** Material override for the snowman mesh. Empty keeps the mesh material. / Материал-оверрайд для сетки. Пусто — материал из сетки. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Mesh",
		meta = (ToolTip = "EN: Material applied to slot 0 of the snowman mesh. Empty = the material saved inside the static mesh.\nRU: Материал для слота 0 сетки снеговика. Пусто = материал, сохранённый в статической сетке."))
	TObjectPtr<UMaterialInterface> SnowmanMaterial;

	/** Offset of the mesh relative to the actor origin. / Смещение сетки относительно центра актёра. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Mesh",
		meta = (ToolTip = "EN: Moves the mesh relative to the actor origin, e.g. to put its base exactly at the actor location.\nRU: Двигает сетку относительно центра актёра, например чтобы её основание стояло ровно в точке актёра."))
	FVector MeshLocationOffset = FVector::ZeroVector;

	/** Rotation of the mesh relative to the actor. / Поворот сетки относительно актёра. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Mesh",
		meta = (ToolTip = "EN: Rotation of the snowman mesh inside the actor.\nRU: Поворот сетки снеговика внутри актёра."))
	FRotator MeshRotationOffset = FRotator::ZeroRotator;

	/** Uniform scale of the snowman. / Общий масштаб снеговика. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Mesh", meta = (ClampMin = "0.01", ClampMax = "50",
		ToolTip = "EN: Uniform scale of the snowman mesh.\nRU: Общий масштаб сетки снеговика."))
	float SnowmanScale = 1.0f;

	// ------------------------------------------------------------------ crow

	/** Which crow class to place on the perch. / Какой класс вороны сажать на присаду. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Crow",
		meta = (ToolTip = "EN: Crow class spawned on the perch at Begin Play (usually BP_Crow).\nRU: Класс вороны, который создаётся на присаде при старте (обычно BP_Crow)."))
	TSubclassOf<ACrowActor> CrowClass;

	/** Spawn the crow automatically on Begin Play. / Создавать ворону автоматически при старте. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Crow",
		meta = (ToolTip = "EN: On: a crow is spawned on the perch when the game starts. Off: place a crow by hand and it will be adopted if it stands close to the perch.\nRU: Вкл: ворона создаётся на присаде при старте игры. Выкл: поставьте ворону вручную — она подхватится, если стоит рядом с присадой."))
	bool bSpawnCrowOnPerch = true;

	/** Make the crow face the same direction as the perch point. / Разворачивать ворону так же, как точку присады. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snowman|Crow",
		meta = (ToolTip = "EN: On: the spawned crow copies the perch rotation. Off: the crow keeps its default rotation.\nRU: Вкл: созданная ворона копирует поворот присады. Выкл: ворона сохраняет свой поворот по умолчанию."))
	bool bAlignCrowToPerch = true;

	/** The crow currently sitting on the snowman (spawned or placed by hand). / Ворона, которая сейчас сидит на снеговике. */
	UPROPERTY(BlueprintReadOnly, Category = "Snowman|Crow",
		meta = (ToolTip = "EN: The crow that currently sits on the perch. Read only, set at runtime.\nRU: Ворона, которая сейчас сидит на присаде. Только чтение, заполняется в рантайме."))
	TObjectPtr<ACrowActor> CrowOnPerch;

	/** World location of the perch. / Мировая позиция присады. */
	UFUNCTION(BlueprintPure, Category = "Snowman",
		meta = (ToolTip = "EN: World location of the perch point.\nRU: Мировая позиция точки присады."))
	FVector GetPerchLocation() const;

	/** Spawn (or move) the crow on the perch. / Создать (или переставить) ворону на присаду. */
	UFUNCTION(BlueprintCallable, Category = "Snowman",
		meta = (ToolTip = "EN: Spawns a new crow on the perch and remembers it. Call it again to replace the current bird.\nRU: Создаёт новую ворону на присаде и запоминает её. Вызовите снова, чтобы заменить текущую птицу."))
	ACrowActor* SpawnCrowOnPerch();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	void ApplyMeshSettings();
};
