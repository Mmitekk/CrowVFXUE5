# UE52VFX Crow & Snowman — Unreal Engine 5.2

Ворона сидит на снеговике, каркает и улетает, когда игрок подходит. Всё настраивается в Blueprint.

[**Русский**](#русский) · [**English**](#english)

![Ворона на снеговике](docs/perched.png)

---

## Русский

### Что это

Плагин для Unreal Engine 5.2: ворона сидит на голове снеговика, при подходе игрока каркает, ждёт заданную задержку и улетает. Все параметры (радиус активации, задержка, скорости, анимации, звуки, аттенюация) редактируются в Blueprint Details.

Логика написана на C++ и полностью открыта для Blueprint: `ACrowActor`, `ASnowmanActor`, `ACrowTrigger` — все `Blueprintable`.

### Требования

- Unreal Engine **5.2**
- Компилятор C++ (Visual Studio 2022 с компонентом «Разработка игр на C++»)
- Пакет **Animal Variety Pack** (ворона: `SK_Crow` + анимации) — должен лежать в `/Game/AnimalVarietyPack/Crow/`

### Установка

**1. Скопировать плагин**

```
<ВашПроект>/Plugins/UE52VFX_Crow/
```

**2. Скопировать контент** — обязательно, иначе Blueprint'ы будут «битыми»

Плагин кладёт ассеты ровно по тем путям, которые прописаны внутри Blueprint'ов:

| Что | Куда |
|---|---|
| `Content/VFX/Crow/` | `<Проект>/Content/VFX/Crow/` |
| `Content/AnimalVarietyPack/Crow/Audio/` | `<Проект>/Content/AnimalVarietyPack/Crow/Audio/` |

Внутри `/Game/VFX/Crow/` должны оказаться:

```
BP_Crow.uasset
BP_CrowTrigger.uasset
BP_Snowman.uasset
ATN_Crow.uasset
M_Carrot.uasset  M_Coal.uasset  M_SnowGround.uasset  M_Snowman.uasset
Audio/SC_Crow_Caw_01..03.uasset
Audio/SC_Crow_WingFlap.uasset
Audio/S_Crow_WingFlap.uasset
```

> **Важно:** пути `/Game/VFX/Crow/...` и `/Game/AnimalVarietyPack/Crow/...` зашиты в сами `.uasset`. Если положить файлы в другую папку — Blueprint'ы не найдут зависимости. Именно поэтому нельзя просто закинуть их в `Content/WinterHunt/Animals/Crow/`.

**3. Включить плагин**

В `WinterHut.uproject` (или любом другом `.uproject`):

```json
"Plugins": [
    {
        "Name": "UE52VFX_Crow",
        "Enabled": true
    }
]
```

Либо в редакторе: **Edit → Plugins → UE52VFX Crow & Snowman → Enabled**.

**4. Собрать**

```bat
"D:\Unreal Projects\UE_5.2\Engine\Build\BatchFiles\Build.bat" <ВашПроект>Editor Win64 Development -Project="<путь>\<Проект>.uproject"
```

Или: **Build → Generate Project Files** и собрать `*.sln` в Visual Studio.

После сборки в `Plugins/UE52VFX_Crow/Binaries/Win64/` появится `UnrealEditor-UE52VFX.dll`.

### Почему модуль называется `UE52VFX`, а папка `UE52VFX_Crow`

Внутри `BP_Crow.uasset` родительский класс записан как `/Script/UE52VFX.CrowActor`. Если назвать модуль иначе (`UE52VFX_Crow`), Unreal не найдёт класс, и Blueprint будет «битым» — его нельзя будет ни открыть, ни поставить на уровень. Поэтому **имя модуля обязано быть `UE52VFX`**. Имя папки плагина при этом любое.

### Использование

**Вариант А — снеговик сам спавнит ворону**

1. Поставьте `BP_Snowman` на уровень
2. В Details укажите `Crow Class` → `BP_Crow`
3. Настройте радиусы: `Outer Activation Radius`, `Middle Activation Radius`, `Inner Activation Radius`
4. `Crow Trigger Delay` — задержка перед взлётом

**Вариант Б — отдельный триггер**

1. Поставьте `BP_CrowTrigger` на уровень
2. `Activation Radius` — радиус срабатывания
3. `Trigger Delay` — задержка
4. `b Spawn Crow` — спавнить ворону автоматически
5. `b One Shot` — однократное срабатывание

### Что настраивается в `BP_Crow`

| Параметр | Описание |
|---|---|
| `Crow Skeletal Mesh` | сетка вороны (`SK_Crow`) |
| `Crow Scale` | масштаб |
| `Idle Animation` | анимация сидения (`ANIM_Crow_IdleLookAround`) |
| `TakeOff Animation` | анимация взлёта (`ANIM_Crow_TakeOff`) |
| `Fly Animation` | анимация полёта (`ANIM_Crow_Fly`) |
| `Caw Sounds` | массив SoundCue карканья |
| `Wing Flap Sounds` | массив SoundCue взмахов |
| `Sound Attenuation` | `ATN_Crow` — громкость и затухание по расстоянию |
| `Activation Radius` | с какого расстояния ворона реагирует |
| `Trigger Delay` | пауза перед взлётом |
| `Take Off Speed` / `Fly Speed` | скорости взлёта и полёта |
| `Climb Height` | высота набора |
| `Max Flight Time` | сколько летит до исчезновения |
| `Despawn Distance` | дистанция, после которой уничтожается |

### API (C++)

```cpp
// Заставить ворону улететь
Crow->TriggerFlee();

// Вернуть в исходное состояние
Crow->ResetToIdle();

// Состояния: Idle → Alarmed → TakingOff → Flying → Gone
```

### Проверено

- Сборка `WinterHutEditor Win64 Development` — успешно, `UnrealEditor-UE52VFX.dll` создан
- Все три Blueprint'а загружаются и **спавнятся на уровень** (headless-проверка в редакторе)
- Компоненты наследуются от нативных классов: `BP_Crow` → `CrowRoot`/`CrowMesh`/`CrowAudio`, `BP_CrowTrigger` → `TriggerSphere`, `BP_Snowman` → `Root`/`HeadBall`/`PerchPoint`

---

## English

### What it is

An Unreal Engine 5.2 plugin: a crow perches on a snowman's head, caws when the player comes close, waits a configurable delay and flies away. Everything (activation radius, delay, speeds, animations, sounds, attenuation) is editable in Blueprint details.

The logic lives in C++ and is fully exposed to Blueprint: `ACrowActor`, `ASnowmanActor`, `ACrowTrigger` are all `Blueprintable`.

### Requirements

- Unreal Engine **5.2**
- A C++ toolchain (Visual Studio 2022 with "Game development with C++")
- **Animal Variety Pack** content (crow `SK_Crow` + animations) at `/Game/AnimalVarietyPack/Crow/`

### Installation

**1. Copy the plugin**

```
<YourProject>/Plugins/UE52VFX_Crow/
```

**2. Copy the content** — required, otherwise the Blueprints are broken

The assets must land on the exact paths baked into the Blueprint files:

| What | Where |
|---|---|
| `Content/VFX/Crow/` | `<Project>/Content/VFX/Crow/` |
| `Content/AnimalVarietyPack/Crow/Audio/` | `<Project>/Content/AnimalVarietyPack/Crow/Audio/` |

`/Game/VFX/Crow/` must contain:

```
BP_Crow.uasset
BP_CrowTrigger.uasset
BP_Snowman.uasset
ATN_Crow.uasset
M_Carrot.uasset  M_Coal.uasset  M_SnowGround.uasset  M_Snowman.uasset
Audio/SC_Crow_Caw_01..03.uasset
Audio/SC_Crow_WingFlap.uasset
Audio/S_Crow_WingFlap.uasset
```

> **Important:** the paths `/Game/VFX/Crow/...` and `/Game/AnimalVarietyPack/Crow/...` are baked into the `.uasset` files. Putting them in another folder breaks the references — that is why dumping them into `Content/WinterHunt/Animals/Crow/` does not work.

**3. Enable the plugin**

In your `.uproject`:

```json
"Plugins": [
    {
        "Name": "UE52VFX_Crow",
        "Enabled": true
    }
]
```

Or in the editor: **Edit → Plugins → UE52VFX Crow & Snowman → Enabled**.

**4. Build**

```bat
"D:\Unreal Projects\UE_5.2\Engine\Build\BatchFiles\Build.bat" <YourProject>Editor Win64 Development -Project="<path>\<Project>.uproject"
```

Or **Build → Generate Project Files**, then build the generated `*.sln`.

After a successful build, `Plugins/UE52VFX_Crow/Binaries/Win64/UnrealEditor-UE52VFX.dll` appears.

### Why the module is called `UE52VFX` and the folder `UE52VFX_Crow`

Inside `BP_Crow.uasset` the parent class is stored as `/Script/UE52VFX.CrowActor`. Name the module anything else (`UE52VFX_Crow`) and Unreal cannot resolve the class — the Blueprint becomes broken and cannot be opened or placed. **The module name must be `UE52VFX`.** The plugin folder name is free.

### Usage

**Option A — snowman spawns the crow**

1. Place `BP_Snowman` in the level
2. Set `Crow Class` to `BP_Crow`
3. Tune `Outer/Middle/Inner Activation Radius`
4. Set `Crow Trigger Delay`

**Option B — standalone trigger**

1. Place `BP_CrowTrigger`
2. `Activation Radius`, `Trigger Delay`
3. `b Spawn Crow` to auto-spawn, `b One Shot` for single use

### Exposed on `BP_Crow`

| Property | Purpose |
|---|---|
| `Crow Skeletal Mesh` | `SK_Crow` |
| `Crow Scale` | uniform scale |
| `Idle Animation` | `ANIM_Crow_IdleLookAround` |
| `TakeOff Animation` | `ANIM_Crow_TakeOff` |
| `Fly Animation` | `ANIM_Crow_Fly` |
| `Caw Sounds` | array of `SoundCue` |
| `Wing Flap Sounds` | array of `SoundCue` |
| `Sound Attenuation` | `ATN_Crow` |
| `Activation Radius` | reaction distance |
| `Trigger Delay` | pause before take-off |
| `Take Off Speed` / `Fly Speed` | speeds |
| `Climb Height` | altitude gain |
| `Max Flight Time` | lifetime of the flight |
| `Despawn Distance` | destroy distance |

### C++ API

```cpp
Crow->TriggerFlee();   // scare it away
Crow->ResetToIdle();   // back to perch
// States: Idle → Alarmed → TakingOff → Flying → Gone
```

### Verified

- `WinterHutEditor Win64 Development` builds clean, `UnrealEditor-UE52VFX.dll` produced
- All three Blueprints load **and spawn into the level** (headless editor check)
- Components inherit from the native classes: `BP_Crow` → `CrowRoot`/`CrowMesh`/`CrowAudio`, `BP_CrowTrigger` → `TriggerSphere`, `BP_Snowman` → `Root`/`HeadBall`/`PerchPoint`

---

## License

MIT. Free to use and modify.
