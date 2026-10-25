// Copyright Epic Games, Inc. All Rights Reserved.
// Perched crow: sits on a snowman, caws from time to time, gets scared by the player and flies away.
// Every gameplay-facing value is exposed to Blueprint / details panel with EN + RU tooltips.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowActor.generated.h"

class USkeletalMeshComponent;
class UAudioComponent;
class USkeletalMesh;
class UMaterialInterface;
class UAnimSequence;
class USoundBase;
class USoundAttenuation;

/** What the crow is doing right now. / Что ворона делает прямо сейчас. */
UENUM(BlueprintType)
enum class ECrowState : uint8
{
	/** Sitting on its perch. / Сидит на присаде. */
	Idle UMETA(DisplayName = "Idle / Сидит"),
	/** Player came close, cawing, waiting for TriggerDelay to take off. / Игрок подошёл, каркает, ждёт TriggerDelay. */
	Alarmed UMETA(DisplayName = "Alarmed / Испугалась"),
	/** Take-off animation is playing, the bird is already moving up. / Играет анимация взлёта, птица уже поднимается. */
	TakingOff UMETA(DisplayName = "Taking Off / Взлетает"),
	/** Flapping away. / Улетает. */
	Flying UMETA(DisplayName = "Flying / Летит"),
	/** Flight is over (hidden or destroyed). / Полёт окончен (скрыта или удалена). */
	Gone UMETA(DisplayName = "Gone / Улетела")
};

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Crow"))
class UE52VFX_API ACrowActor : public AActor
{
	GENERATED_BODY()

public:
	ACrowActor();

	// ------------------------------------------------------------------ components

	/** Plain root, keeps the actor transform independent from the mesh offset. / Корень актёра, отделяет трансформ от смещения меша. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components",
		meta = (ToolTip = "EN: Root scene component of the crow. Do not edit its transform in the Blueprint, use the actor transform instead.\nRU: Корневой компонент вороны. Не меняйте его трансформ в блюпринте, используйте трансформ самого актёра."))
	TObjectPtr<USceneComponent> CrowRoot;

	/** Skeletal mesh of the crow. / Скелетный меш вороны. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components",
		meta = (ToolTip = "EN: Skeletal mesh component. Its rotation is driven by the Auto Align Mesh Forward option and Mesh Rotation Offset.\nRU: Компонент скелетного меша. Его поворот задаётся опцией Auto Align Mesh Forward и смещением Mesh Rotation Offset."))
	TObjectPtr<USkeletalMeshComponent> CrowMesh;

	/** Caw / voice audio: the periodic caws while the bird sits on the perch. / Звук голоса: периодическое карканье, пока птица сидит. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components",
		meta = (ToolTip = "EN: Audio component used for the idle caws (Idle Caw Sounds).\nRU: Аудиокомпонент для карканья на присаде (Idle Caw Sounds)."))
	TObjectPtr<UAudioComponent> CrowAudio;

	/** Alarm audio: the caw played when the player scares the bird. / Звук тревоги: карканье в момент испуга. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components",
		meta = (ToolTip = "EN: Audio component used for the alarm caws (Alarm Sounds). Has its own volume and attenuation.\nRU: Аудиокомпонент для карканья при испуге (Alarm Sounds). Имеет собственную громкость и затухание."))
	TObjectPtr<UAudioComponent> AlarmAudio;

	/** Flight audio: wing flaps. / Звук полёта: хлопки крыльев. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crow|Components",
		meta = (ToolTip = "EN: Audio component used for the take-off / flight sounds (Take Off Sounds). Has its own volume and attenuation.\nRU: Аудиокомпонент для звуков взлёта и полёта (Take Off Sounds). Имеет собственную громкость и затухание."))
	TObjectPtr<UAudioComponent> FlightAudio;

	// ------------------------------------------------------------------ mesh

	/** Skeletal mesh to use. / Какой скелетный меш использовать. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh",
		meta = (ToolTip = "EN: Skeletal mesh of the bird. Leave empty to keep whatever the mesh component already has.\nRU: Скелетный меш птицы. Оставьте пустым, чтобы сохранить меш, уже назначенный компоненту."))
	TObjectPtr<USkeletalMesh> CrowSkeletalMesh;

	/** Optional material override for slot 0. / Необязательный материал-оверрайд для слота 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh",
		meta = (ToolTip = "EN: Material applied to slot 0. Empty = the material saved inside the skeletal mesh.\nRU: Материал для слота 0. Пусто = материал, сохранённый в скелетном меше."))
	TObjectPtr<UMaterialInterface> CrowMaterialOverride;

	/** Offset of the mesh inside the actor (use it to put the feet exactly on the perch). / Смещение меша внутри актёра (поставьте лапы ровно на присаду). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh",
		meta = (ToolTip = "EN: Offset of the mesh relative to the actor origin. Use Z to put the feet exactly on the perch.\nRU: Смещение меша относительно центра актёра. По Z подгоните так, чтобы лапы стояли ровно на присаде."))
	FVector MeshRelativeLocation = FVector::ZeroVector;

	/** Extra rotation added on top of the automatic forward alignment. / Дополнительный поворот поверх авто-выравнивания вперёд. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh",
		meta = (ToolTip = "EN: Extra rotation of the mesh. Yaw is added to the automatic forward alignment, pitch/roll are applied as they are. Edit this instead of the component rotation.\nRU: Дополнительный поворот меша. Yaw прибавляется к авто-выравниванию вперёд, pitch/roll применяются как есть. Меняйте это значение, а не поворот самого компонента."))
	FRotator MeshRotationOffset = FRotator::ZeroRotator;

	/** Automatically rotate the mesh so the bird looks along the actor forward axis. / Автоматически поворачивать меш, чтобы птица смотрела по оси «вперёд» актёра. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh",
		meta = (ToolTip = "EN: On: the yaw is computed from the skeleton (root to head direction) so the bird always faces the actor forward axis, whatever axis the asset was authored with. Turn off only if you want to set the yaw by hand.\nRU: Вкл: поворот вычисляется по скелету (направление от корня к голове), поэтому птица всегда смотрит по оси «вперёд» актёра, как бы меш ни был повёрнут в ассете. Выключайте, только если хотите задать поворот вручную."))
	bool bAutoAlignMeshForward = true;

	/** Uniform scale of the crow. / Общий масштаб вороны. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Mesh", meta = (ClampMin = "0.05", ClampMax = "10",
		ToolTip = "EN: Uniform scale of the bird.\nRU: Общий масштаб птицы."))
	float CrowScale = 1.0f;

	// ------------------------------------------------------------------ animation

	/** Looping animation played while the crow is sitting. / Зацикленная анимация, пока птица сидит. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation",
		meta = (ToolTip = "EN: Idle animation (looping) played while the bird sits on the perch.\nRU: Анимация покоя (зацикленная), играет пока птица сидит на присаде."))
	TObjectPtr<UAnimSequence> IdleAnimation;

	/** One-shot animation played at take-off. / Одноразовая анимация взлёта. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation",
		meta = (ToolTip = "EN: Take-off animation (played once).\nRU: Анимация взлёта (проигрывается один раз)."))
	TObjectPtr<UAnimSequence> TakeOffAnimation;

	/** Looping animation played during the flight. / Зацикленная анимация полёта. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation",
		meta = (ToolTip = "EN: Flight animation (looping, plays while the bird flies away).\nRU: Анимация полёта (зацикленная, играет пока птица улетает)."))
	TObjectPtr<UAnimSequence> FlyAnimation;

	/** Play rate of the idle animation. / Скорость анимации покоя. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation", meta = (ClampMin = "0.05", ClampMax = "4",
		ToolTip = "EN: Play rate of the idle animation. 1 = authored speed.\nRU: Скорость анимации покоя. 1 = как в ассете."))
	float IdlePlayRate = 1.0f;

	/** Play rate of the flight animation (wing beat speed). / Скорость анимации полёта (частота взмахов). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation", meta = (ClampMin = "0.05", ClampMax = "4",
		ToolTip = "EN: Play rate of the flight animation, i.e. the wing beat speed.\nRU: Скорость анимации полёта, то есть частота взмахов крыльев."))
	float FlyPlayRate = 1.0f;

	/** Time scale of the take-off animation. Values > 1 make the jump quicker. / Скорость анимации взлёта. Больше 1 — прыжок быстрее. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Animation", meta = (ClampMin = "0.1", ClampMax = "4",
		ToolTip = "EN: Time scale of the take-off animation. Above 1 the jump is quicker and the flight starts earlier.\nRU: Скорость анимации взлёта. Больше 1 — прыжок быстрее, полёт начинается раньше."))
	float TakeOffAnimationTimeScale = 1.0f;

	// ------------------------------------------------------------------ sound | idle caw

	/** Caws played from time to time while the bird sits on the perch. / Карканье, которое играет время от времени, пока птица сидит. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Idle Caw",
		meta = (ToolTip = "EN: List of caw sounds played periodically while the bird sits. One is picked at random.\nRU: Список звуков карканья, которые играют периодически, пока птица сидит. Один выбирается случайно."))
	TArray<TObjectPtr<USoundBase>> IdleCawSounds;

	/** Caw periodically while idle. / Каркать периодически, пока сидит. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Idle Caw",
		meta = (ToolTip = "EN: Play a random sound from Idle Caw Sounds every N seconds while the bird is idle.\nRU: Проигрывать случайный звук из Idle Caw Sounds каждые N секунд, пока птица сидит."))
	bool bCawWhileIdle = true;

	/** Minimum pause (s) between two idle caws. / Минимальная пауза (с) между карканьями. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Idle Caw", meta = (ClampMin = "0.2", ClampMax = "120",
		ToolTip = "EN: Minimum interval in seconds between two idle caws. The real interval is random between Min and Max.\nRU: Минимальный интервал в секундах между двумя карканьями. Реальный интервал — случайный между Min и Max."))
	float IdleCawIntervalMin = 3.0f;

	/** Maximum pause (s) between two idle caws. / Максимальная пауза (с) между карканьями. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Idle Caw", meta = (ClampMin = "0.2", ClampMax = "300",
		ToolTip = "EN: Maximum interval in seconds between two idle caws.\nRU: Максимальный интервал в секундах между двумя карканьями."))
	float IdleCawIntervalMax = 9.0f;

	/** Volume of the idle caws. / Громкость карканья на присаде. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Idle Caw", meta = (ClampMin = "0", ClampMax = "4",
		ToolTip = "EN: Volume multiplier of the idle caws.\nRU: Множитель громкости карканья на присаде."))
	float IdleCawVolume = 1.0f;

	/** Attenuation of the idle caws. / Затухание карканья на присаде. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Idle Caw",
		meta = (ToolTip = "EN: Sound attenuation used by the idle caws (radius, falloff, spatialization).\nRU: Затухание звука для карканья на присаде (радиус, спад, пространственность)."))
	TObjectPtr<USoundAttenuation> IdleCawAttenuation;

	// ------------------------------------------------------------------ sound | alarm

	/** Caws played when the player scares the bird. / Карканье в момент испуга. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Alarm",
		meta = (ToolTip = "EN: Sounds played the moment the bird is scared (alarm caw). One is picked at random.\nRU: Звуки, которые играют в момент испуга (тревожное карканье). Один выбирается случайно."))
	TArray<TObjectPtr<USoundBase>> AlarmedSounds;

	/** Volume of the alarm caws. / Громкость тревожного карканья. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Alarm", meta = (ClampMin = "0", ClampMax = "4",
		ToolTip = "EN: Volume multiplier of the alarm caws.\nRU: Множитель громкости тревожного карканья."))
	float AlarmVolume = 1.2f;

	/** Attenuation of the alarm caws. / Затухание тревожного карканья. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Alarm",
		meta = (ToolTip = "EN: Sound attenuation used by the alarm caws.\nRU: Затухание звука для тревожного карканья."))
	TObjectPtr<USoundAttenuation> AlarmAttenuation;

	// ------------------------------------------------------------------ sound | take off

	/** Wing flaps played while leaving the perch. / Хлопки крыльев при взлёте. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Take Off",
		meta = (ToolTip = "EN: Sounds played when the bird leaves the perch (wing flaps). One is picked at random.\nRU: Звуки, которые играют при взлёте (хлопки крыльев). Один выбирается случайно."))
	TArray<TObjectPtr<USoundBase>> TakeOffSounds;

	/** Volume of the take-off sounds. / Громкость звуков взлёта. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Take Off", meta = (ClampMin = "0", ClampMax = "4",
		ToolTip = "EN: Volume multiplier of the take-off sounds.\nRU: Множитель громкости звуков взлёта."))
	float TakeOffVolume = 1.0f;

	/** Attenuation of the take-off sounds. / Затухание звуков взлёта. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound|Take Off",
		meta = (ToolTip = "EN: Sound attenuation used by the take-off sounds.\nRU: Затухание звука для звуков взлёта."))
	TObjectPtr<USoundAttenuation> TakeOffAttenuation;

	/** Random pitch variation applied to every sound, in +/- semitone-ish fraction. / Случайное изменение высоты тона для каждого звука. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound", meta = (ClampMin = "0", ClampMax = "0.5",
		ToolTip = "EN: Random pitch variation of every played sound. 0.1 = +/-10%, makes repeated caws sound less mechanical.\nRU: Случайное изменение высоты тона каждого звука. 0.1 = +/-10%, чтобы повторяющееся карканье не звучало одинаково."))
	float SoundPitchJitter = 0.08f;

	/** Spatialize the sounds (off = flat 2D sound, useful for testing). / Пространственный звук (выкл = плоский 2D, удобно для теста). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Sound",
		meta = (ToolTip = "EN: When on, sounds are spatialized through the attenuation assets. Turn off for a flat 2D sound while testing.\nRU: Если включено, звуки пространственные и используют ассеты затухания. Выключите для плоского 2D-звука при тестах."))
	bool bSpatializeSounds = true;

	// ------------------------------------------------------------------ reaction

	/** The crow watches its own distance to the player. / Ворона сама следит за дистанцией до игрока. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction",
		meta = (ToolTip = "EN: On: the actor checks the distance to the player itself. Off: only an external trigger (BP_CrowTrigger) scares it.\nRU: Вкл: актёр сам проверяет дистанцию до игрока. Выкл: пугает только внешний триггер (BP_CrowTrigger)."))
	bool bUseProximityTrigger = true;

	/** Distance (cm) at which the player scares the bird. / Дистанция (см), на которой игрок пугает птицу. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "10000",
		ToolTip = "EN: Radius in cm around the bird. When the player comes closer, the crow gets scared.\nRU: Радиус в сантиметрах вокруг птицы. Когда игрок подходит ближе, ворона пугается."))
	float ActivationRadius = 550.0f;

	/** Delay (s) between the alarm and the jump; the crow caws during this time. / Задержка (с) между испугом и прыжком; ворона каркает в это время. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "10",
		ToolTip = "EN: Delay in seconds between the alarm caw and the take-off.\nRU: Задержка в секундах между тревожным карканьем и взлётом."))
	float TriggerDelay = 0.35f;

	/** React to the player pawn only. When off, any pawn scares the crow. / Реагировать только на игрока. Выкл — любой Pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction",
		meta = (ToolTip = "EN: On: only the player pawn scares the bird. Off: any pawn (animals, NPCs) does.\nRU: Вкл: пугает только пешка игрока. Выкл: любая пешка (животные, NPC)."))
	bool bOnlyReactToPlayer = true;

	/** Slowly turn the head/body towards the player before he gets too close. / Медленно поворачиваться к игроку до того, как он подойдёт вплотную. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction",
		meta = (ToolTip = "EN: On: the bird turns towards the player while he is inside Look At Radius. Off: it keeps its authored rotation.\nRU: Вкл: птица поворачивается к игроку, пока он в радиусе Look At Radius. Выкл: сохраняет исходный поворот."))
	bool bLookAtPlayer = true;

	/** Distance (cm) within which the crow starts tracking the player. / Дистанция (см), в которой ворона начинает следить за игроком. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "10000",
		ToolTip = "EN: Radius in cm within which the bird starts turning towards the player.\nRU: Радиус в сантиметрах, внутри которого птица начинает поворачиваться к игроку."))
	float LookAtRadius = 1600.0f;

	/** Turn speed while tracking the player (deg/s). / Скорость поворота к игроку (град/с). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction", meta = (ClampMin = "0", ClampMax = "720",
		ToolTip = "EN: Turning speed in degrees per second while the bird tracks the player.\nRU: Скорость поворота в градусах в секунду, пока птица следит за игроком."))
	float LookAtTurnSpeed = 70.0f;

	/** Draw the activation radius in the level (green = idle, red = triggered). / Рисовать радиус срабатывания (зелёный = спокойна, красный = сработала). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Reaction",
		meta = (ToolTip = "EN: Draws the activation radius in the level viewport. Green while idle, red after the alarm. Debug only.\nRU: Рисует радиус срабатывания во вьюпорте. Зелёный пока спокойна, красный после испуга. Только для отладки."))
	bool bDrawActivationRadius = false;

	// ------------------------------------------------------------------ flight

	/** Fly away from the player instead of straight ahead. / Улетать от игрока, а не по прямой. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight",
		meta = (ToolTip = "EN: On: the escape direction points away from whoever scared the bird. Off: the bird flies along its own forward axis.\nRU: Вкл: направление улёта — от того, кто испугал. Выкл: птица летит по своей оси «вперёд»."))
	bool bFlyAwayFromPlayer = true;

	/** Random spread (deg) added to the escape direction so repeated runs look different. / Случайный разброс (град) направления улёта. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "180",
		ToolTip = "EN: Random spread in degrees added to the escape direction, so repeated runs do not look identical.\nRU: Случайный разброс в градусах, добавляемый к направлению улёта, чтобы повторы не выглядели одинаково."))
	float DirectionRandomSpreadDeg = 20.0f;

	/** Initial upward speed (cm/s) of the jump. / Начальная вертикальная скорость прыжка (см/с). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "3000",
		ToolTip = "EN: Initial upward speed of the jump, in cm/s.\nRU: Начальная скорость подъёма при прыжке, см/с."))
	float TakeOffVerticalSpeed = 420.0f;

	/** Cruise speed (cm/s) of the flight. / Крейсерская скорость полёта (см/с). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "5000",
		ToolTip = "EN: Cruise speed of the flight in cm/s.\nRU: Крейсерская скорость полёта, см/с."))
	float FlySpeed = 900.0f;

	/** Acceleration (cm/s^2) towards the cruising speed. / Ускорение (см/с^2) до крейсерской скорости. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "10", ClampMax = "10000",
		ToolTip = "EN: Acceleration towards the cruise speed, in cm/s^2. Bigger = snappier take-off.\nRU: Ускорение до крейсерской скорости, см/с^2. Больше — резвее взлёт."))
	float FlyAcceleration = 900.0f;

	/** How high (cm) the bird climbs before levelling out. / До какой высоты (см) птица набирает высоту. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "10000",
		ToolTip = "EN: Height in cm the bird climbs before it levels out.\nRU: Высота в сантиметрах, которую птица набирает, прежде чем выровняться."))
	float ClimbHeight = 800.0f;

	/** Vertical speed (cm/s) while climbing. / Вертикальная скорость подъёма (см/с). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "2000",
		ToolTip = "EN: Climb speed in cm/s while the bird is below Climb Height.\nRU: Скорость подъёма, см/с, пока птица ниже Climb Height."))
	float ClimbSpeed = 320.0f;

	/** How long (s) the bird stays in the air before it is removed. / Сколько секунд птица в воздухе до удаления. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0.5", ClampMax = "120",
		ToolTip = "EN: Maximum flight time in seconds. When it expires the bird disappears (or is destroyed).\nRU: Максимальное время полёта в секундах. По истечении птица исчезает (или удаляется)."))
	float MaxFlightTime = 12.0f;

	/** Distance (cm) from the perch after which the bird is removed. 0 = ignore. / Дистанция (см) от присады, после которой птица исчезает. 0 = не учитывать. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight", meta = (ClampMin = "0", ClampMax = "100000",
		ToolTip = "EN: Distance in cm from the perch after which the bird disappears. 0 = ignore the distance.\nRU: Дистанция в сантиметрах от присады, после которой птица исчезает. 0 = не учитывать."))
	float DespawnDistance = 6000.0f;

	/** Destroy the actor once the flight is over. Off = just hide it. / Удалить актёра после полёта. Выкл = просто скрыть. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight",
		meta = (ToolTip = "EN: On: the actor is destroyed after the flight. Off: it is only hidden, which makes repeated tests easier.\nRU: Вкл: актёр удаляется после полёта. Выкл: только скрывается, что удобнее при повторных тестах."))
	bool bDestroyAfterFlight = true;

	/** Turn off collision on take-off so the bird never pushes the player. / Выключить коллизию на взлёте. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crow|Flight",
		meta = (ToolTip = "EN: Disables collision on take-off so the bird can never push the player or props.\nRU: Выключает коллизию на взлёте, чтобы птица не толкала игрока и предметы."))
	bool bDisableCollisionOnTakeOff = true;

	// ------------------------------------------------------------------ state

	/** Current state of the bird. / Текущее состояние птицы. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Crow|State",
		meta = (ToolTip = "EN: Current state of the bird (idle, alarmed, taking off, flying, gone). Read only.\nRU: Текущее состояние птицы (сидит, испугалась, взлетает, летит, улетела). Только чтение."))
	ECrowState State = ECrowState::Idle;

	// ------------------------------------------------------------------ api

	/** Scare the crow: it caws, waits TriggerDelay and flies away. Safe to call many times. / Испугать ворону: каркнет, подождёт TriggerDelay и улетит. Можно вызывать многократно. */
	UFUNCTION(BlueprintCallable, Category = "Crow",
		meta = (ToolTip = "EN: Scares the bird: it caws, waits Trigger Delay and flies away. Does nothing when the bird is already flying.\nRU: Пугает птицу: она каркает, ждёт Trigger Delay и улетает. Ничего не делает, если птица уже улетает."))
	void TriggerFlee(AActor* InstigatorActor = nullptr);

	/** Scare the bird into a specific world direction, optionally after a delay and silently. / Испугать ворону в заданном направлении, при желании с задержкой и без крика. */
	UFUNCTION(BlueprintCallable, Category = "Crow",
		meta = (ToolTip = "EN: Scares the bird and makes it fly exactly along Direction (world space), starting after Start Delay (0 or less = right now). bPlayAlarmCaw = false makes it take off silently. Used by the flock actor.\nRU: Пугает птицу и заставляет лететь точно по Direction (мировое направление), начиная через Start Delay (0 или меньше = сразу). bPlayAlarmCaw = false — взлёт без карканья. Используется актёром стаи."))
	void ScareInDirection(AActor* InstigatorActor, FVector Direction, float StartDelay = -1.0f, bool bPlayAlarmCaw = true);

	/** Put the bird back on its perch (useful for testing and for respawning). / Вернуть птицу на присаду (для тестов и респавна). */
	UFUNCTION(BlueprintCallable, Category = "Crow",
		meta = (ToolTip = "EN: Puts the bird back on its perch and restarts the idle behaviour. Use it to replay the scene without restarting the game.\nRU: Возвращает птицу на присаду и перезапускает поведение покоя. Удобно, чтобы проиграть сцену заново без перезапуска игры."))
	void ResetToIdle();

	/** Current state. / Текущее состояние. */
	UFUNCTION(BlueprintPure, Category = "Crow",
		meta = (ToolTip = "EN: Returns the current state of the bird.\nRU: Возвращает текущее состояние птицы."))
	ECrowState GetCrowState() const { return State; }

	/** Distance to the pawn the crow reacts to, or -1 when there is none. / Дистанция до пешки, на которую реагирует ворона, или -1. */
	UFUNCTION(BlueprintPure, Category = "Crow",
		meta = (ToolTip = "EN: Distance in cm to the pawn the bird reacts to, or -1 when there is none.\nRU: Дистанция в сантиметрах до пешки, на которую реагирует птица, или -1, если её нет."))
	float GetDistanceToActivationTarget() const;

	/** Yaw offset that aligns the mesh forward axis with the actor forward axis (0 when it cannot be computed). / Yaw, выравнивающий меш по оси «вперёд» (0, если вычислить нельзя). */
	UFUNCTION(BlueprintPure, Category = "Crow",
		meta = (ToolTip = "EN: The yaw offset computed from the skeleton that makes the bird face the actor forward axis. Debug helper.\nRU: Смещение по yaw, вычисленное по скелету, чтобы птица смотрела по оси «вперёд» актёра. Вспомогательная функция."))
	float GetComputedMeshYawOffset() const;

	/** True once the bird has flown away at least once. Not cleared by Reset To Idle. / True, если птица уже хотя бы раз улетела. Reset To Idle его не сбрасывает. */
	UPROPERTY(BlueprintReadOnly, Category = "Crow|State",
		meta = (ToolTip = "EN: True once the bird has flown away at least once. It stays true after Reset To Idle, so 'the bird is gone' logic is easy to hook.\nRU: True, если птица уже хотя бы раз улетела. Остаётся true после Reset To Idle, чтобы удобно было вешать логику «птица улетела»."))
	bool bHasFled = false;

	/** Has the bird already flown away at least once. / Птица уже улетала хотя бы раз. */
	UFUNCTION(BlueprintPure, Category = "Crow",
		meta = (ToolTip = "EN: True when the bird has already flown away at least once. Read it from any Blueprint.\nRU: True, когда птица уже улетала хотя бы раз. Можно читать из любого блюпринта."))
	bool HasFled() const { return bHasFled; }

	/** Scared. Can be overridden in Blueprint (right click → Add Call to Parent Function). / Испуг. Переопределяется в блюпринте. */
	UFUNCTION(BlueprintNativeEvent, Category = "Crow",
		meta = (ToolTip = "EN: Fired when the bird is scared, before the take-off. Override it in the Blueprint; call the parent function to also run the C++ default.\nRU: Срабатывает, когда птица испугалась, до взлёта. Переопределяйте в блюпринте; вызовите родительскую функцию, чтобы выполнился и код C++."))
	void OnCrowAlarmed(AActor* InstigatorActor);

	/** Bird leaves the perch. Can be overridden in Blueprint. / Птица покидает присаду. Переопределяется в блюпринте. */
	UFUNCTION(BlueprintNativeEvent, Category = "Crow",
		meta = (ToolTip = "EN: Fired at take-off. Override it in the Blueprint; call the parent function to also run the C++ default.\nRU: Срабатывает при взлёте. Переопределяйте в блюпринте; вызовите родительскую функцию, чтобы выполнился и код C++."))
	void OnCrowTakeOff();

	/** Take-off animation switches to the flight loop. Can be overridden in Blueprint. / Переход на анимацию полёта. Переопределяется в блюпринте. */
	UFUNCTION(BlueprintNativeEvent, Category = "Crow",
		meta = (ToolTip = "EN: Fired when the take-off animation switches to the flight loop. Override it in the Blueprint; call the parent function to also run the C++ default.\nRU: Срабатывает при переходе с анимации взлёта на цикл полёта. Переопределяйте в блюпринте; вызовите родительскую функцию, чтобы выполнился и код C++."))
	void OnCrowFlyAway();

	/** The bird has flown away: fired at the end of every flight, before the actor is destroyed or hidden. / Птица улетела: срабатывает в конце полёта, до удаления или скрытия актёра. */
	UFUNCTION(BlueprintNativeEvent, Category = "Crow",
		meta = (ToolTip = "EN: Fired when the flight is over, before the actor is destroyed or hidden. This is the 'the bird has flown away' hook: spawn a reward, advance a quest, play a sound. It fires again if the bird is put back on the perch and flies away again; use Has Fled to check whether this is the first time. Override it in the Blueprint; call the parent function to also run the C++ default.\nRU: Срабатывает, когда полёт закончен, до удаления или скрытия актёра. Это хук «птица улетела»: выдать награду, продвинуть квест, проиграть звук. Сработает снова, если вернуть птицу на присаду и спугнуть повторно; чтобы понять, что это первый раз, используйте Has Fled. Переопределяйте в блюпринте; вызовите родительскую функцию, чтобы выполнился и код C++."))
	void OnCrowFled();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ApplyMeshSettings();
	void PlayAnimationSequence(UAnimSequence* Anim, bool bLoop, float Rate);
	void PlayIdle();
	void EnterTakeOff();
	void EnterFlying();
	void FinishFlight();
	void PlayRandomSound(UAudioComponent* Component, const TArray<TObjectPtr<USoundBase>>& Sounds,
		float Volume, USoundAttenuation* Attenuation);
	void ScheduleNextIdleCaw();
	void HandleIdleCaw();
	void StopAllSounds();
	APawn* FindReactionPawn() const;
	void UpdateIdle(float DeltaSeconds);
	void UpdateFlying(float DeltaSeconds);

	FTimerHandle StateTimer;
	FTimerHandle IdleCawTimer;

	/** Transform the crow was placed at, used by ResetToIdle. */
	FTransform IdleTransform;

	/** Who scared the bird (used for the escape direction). */
	TWeakObjectPtr<AActor> AlarmInstigator;

	/** Set by ScareInDirection: fly exactly there instead of away from the player. */
	bool bHasFlightDirectionOverride = false;
	FVector FlightDirectionOverride = FVector::ForwardVector;

	/** Set by ScareInDirection: play the alarm caw or take off silently. */
	bool bPlayAlarmSound = true;

	FTimerHandle ScareDelayTimer;

	FVector FlightDirection = FVector::ForwardVector;
	FVector Velocity = FVector::ZeroVector;
	FVector FlightStartLocation = FVector::ZeroVector;
	float FlightElapsed = 0.0f;
};
