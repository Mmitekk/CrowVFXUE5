# UE52VFX Crow & Snowman Plugin

[English](#english) | [Русский](#русский)

---

## English

### Overview

**UE52VFX_Crow** is a Unreal Engine 5.2 plugin that implements a crow sitting on a snowman, taking off when the player approaches, and flying away. 

### Features

- **Crow AI** with 5 states: Idle → Alarmed → TakingOff → Flying → Landing
- **Snowman** with 3 activation radius zones (outer/middle/inner)
- **Trigger system** (one-shot or multi-shot)
- All parameters editable in Blueprint Details
- Sound system with attenuation (80cm inner sphere, 15m falloff)

### Installation

1. Place `Plugins/UE52VFX_Crow/` in your project's `Plugins/` directory
2. Enable via **Edit → Plugins → UE52VFX_Crow**
3. Restart editor and rebuild solution

### ⚠️ Content Required

The plugin depends on Content assets copied from the UE52VFX project. Without these, Blueprints will fail to load. Copy the following folders:

- `Content/Crow/` - crow skeleton, 17 animations, textures (BaseColor, Normal, ORM, Specular), material M_Crow
- `Content/Crow/Audio/` - SC_Crow_Caw_01/02/03 sound cues, SC_Crow_WingFlap, S_Crow_WingFlap
- `Content/Snowman/` - materials M_SnowBody, M_Carrot, M_Coal, M_SnowGround
- `Content/Maps/L_CrowSnowman.umap` - level with snowman, crow on head, trigger
- `Content/Blueprints/BP_*.uasset` - BP_Crow, BP_Snowman, BP_CrowTrigger

### Usage

**Spawning a Crow on Snowman:**
1. Place `ASnowmanActor` in your level
2. Set `Crow Class` to `BP_Crow` or `ACrowActor` subclass
3. Assign materials: `M_SnowBody`, `M_Coal`, `M_Carrot`, `M_SnowGround`
4. Configure radii: `OuterActivationRadius`, `MiddleActivationRadius`, `InnerActivationRadius`
5. Set `CrowTriggerDelay` for flight delay
6. Play - crow takes off when player enters middle radius

**Using Trigger Volume:**
1. Place `ACrowTrigger` in level
2. Set `ActivationRadius` and `TriggerDelay`
3. Choose `bSpawnCrow` for auto-spawn
4. Choose `bOneShot` for single use or allow multiple activations
5. Assign `CrowClass` for specific crow class

### Blueprint Examples

**Trigger crow flight:**
```blueprint
Event BeginPlay
    -> Get ACrowTrigger
    -> Call TriggerCrow()
```

**Check if flying:**
```blueprint
Event Tick
    -> Get ACrowTrigger
    -> Call IsCrowFlying()
    -> Branch (result)
       -> Do flying logic
```

**Reset to idle:**
```blueprint
Event AnyKey
    -> Get ASnowmanActor
    -> Call ResetState()
```

### Project Structure

```
UE52VFX_Crow/
├─ Source/UE52VFX_Crow/       ← C++ source files
│  ├─ CrowActor.h/.cpp
│  ├─ SnowmanActor.h/.cpp
│  ├─ CrowTrigger.h/.cpp
│  ├─ CrowAssetLibrary.h/.cpp
│  └─ UE52VFX_Crow.Build.cs
├─ uplugin/UE52VFX_Crow.uplugin
├─ Content/                   ← Blueprints, materials, sounds
└─ README.md
```

### License

Free to use and modify. Attribution appreciated but not required.

---

## Русский

### Обзор

**UE52VFX_Crow** — плагин для Unreal Engine 5.2, реализующий ворону, сидящую на снеговике, которая улетает когда к ней подходит игрок.

### Возможности

- **AI воровы** с 5 состояниями: Idle → Alarmed → TakingOff → Flying → Landing
- **Снеговик** с 3 зонами активации (внешний/средний/внутренний)
- **Система триггеров** (однократный/многократный)
- Все параметры редактируются в Details Blueprint
- Звуковая система с аттенюацией (80см внутренняя сфера, 15м затухание)

### Установка

1. Положите `Plugins/UE52VFX_Crow/` в директорию `Plugins/` вашего проекта
2. Включите плагин в **Edit → Plugins → UE52VFX_Crow**
3. Перезапустите редактор и пересоберите решение

### ⚠️ Необходимый контент

Плагин зависит от ассетов, скопированных из проекта UE52VFX. Без этих файлов Blueprint-ы не загрузятся. Скопируйте следующие папки:

- `Content/Crow/` — скелет, 17 анимаций, текстуры (BaseColor, Normal, ORM, Specular), материал M_Crow
- `Content/Crow/Audio/` — звуковые файлы SC_Crow_Caw_01/02/03, SC_Crow_WingFlap, S_Crow_WingFlap
- `Content/Snowman/` — материалы M_SnowBody, M_Carrot, M_Coal, M_SnowGround
- `Content/Maps/L_CrowSnowman.umap` — уровень: снеговик + ворона на голове + триггер
- `Content/Blueprints/BP_*.uasset` — BP_Crow, BP_Snowman, BP_CrowTrigger

### Использование

**Спавн вороны на снеговике:**
1. Положите `ASnowmanActor` в уровень
2. Установите свойство `Crow Class` в `BP_Crow` (или класс `ACrowActor` наследник)
3. Назначьте материалы: `M_SnowBody`, `M_Coal`, `M_Carrot`, `M_SnowGround`
4. Настройте радиусы: `OuterActivationRadius`, `MiddleActivationRadius`, `InnerActivationRadius`
5. Установите `CrowTriggerDelay` для задержки перед полетом
6. Поиграйте — ворона взлетит когда игрок займет средний радиус

**Использование объемного триггера:**
1. Положите `ACrowTrigger` в уровень
2. Установите `ActivationRadius` и `TriggerDelay`
3. Выберите `bSpawnCrow` для автоматического спавна
4. Выберите `bOneShot` для однократного срабатывания или разрешите многократные активации
5. Назначьте `CrowClass` для конкретного класса вороны

### Примеры в Blueprints

**Активация полета:**
```blueprint
Event BeginPlay
    -> Get ACrowTrigger
    -> Call TriggerCrow()
```

**Проверка полета:**
```blueprint
Event Tick
    -> Get ACrowTrigger
    -> Call IsCrowFlying()
    -> Branch (result)
       -> Do flying logic
```

**Возврат в idle:**
```blueprint
Event AnyKey
    -> Get ASnowmanActor
    -> Call ResetState()
```

### Структура проекта

```
UE52VFX_Crow/
├─ Source/UE52VFX_Crow/       ← C++ source files
│  ├─ CrowActor.h/.cpp
│  ├─ SnowmanActor.h/.cpp
│  ├─ CrowTrigger.h/.cpp
│  ├─ CrowAssetLibrary.h/.cpp
│  └─ UE52VFX_Crow.Build.cs
├─ uplugin/UE52VFX_Crow.uplugin
├─ Content/                   ← Blueprints, materials, sounds
└─ README.md
```

### Лицензия

Бесплатен для использования и модификации. Атрибуция приветствуется, но не требуется.

---

## Installation

### Method 1: Via Plugins Menu
1. Place the `UE52VFX_Crow` folder in your project's `Plugins/` directory
2. Enable the plugin in **Edit → Plugins → UE52VFX_Crow**
3. Restart Unreal Engine editor
4. The C++ classes will compile automatically

### Method 2: Manual Setup
1. Copy `Plugins/UE52VFX_Crow/` into your project's `Plugins/` folder
2. Add `UE52VFX_Crow` to your module's `Build.cs`:
   ```csharp
   PublicDependencyNames.Add("UE52VFX_Crow");
   ```
3. Rebuild the solution (Build → Generate Project Files → Build)

## Usage

### Spawning a Crow on Snowman
1. Place `ASnowmanActor` in your level
2. Set the `Crow Class` property to `BP_Crow` (or `ACrowActor` subclass)
3. Assign materials: `M_SnowBody`, `M_Coal`, `M_Carrot`, `M_SnowGround`
4. Configure radius values: `OuterActivationRadius`, `MiddleActivationRadius`, `InnerActivationRadius`
5. Set `CrowTriggerDelay` for the delay before flight
6. Play - when player enters the middle radius, the crow will take off

### Using the Trigger Volume
1. Place `ACrowTrigger` in your level
2. Set `ActivationRadius` for the trigger area
3. Set `TriggerDelay` for the delay
4. Choose `bSpawnCrow` to automatically spawn a crow
5. Choose `bOneShot` for single-use trigger or allow multiple activations
6. Assign `CrowClass` if you want a specific crow class

## Content Included

### C++ Classes
- `ACrowActor` - Main crow AI actor
- `ASnowmanActor` - Snowman with crow spawning
- `ACrowTrigger` - Trigger volume for crow activation
- `UCrowAssetLibrary` - Asset creation helper
- `UCrowSoundLibrary` - Sound management

### Blueprints (generated from C++)
- `BP_Crow` - Editable blueprint based on `ACrowActor`
- `BP_Snowman` - Editable blueprint based on `ASnowmanActor`
- `BP_CrowTrigger` - Editable blueprint based on `ACrowTrigger`

### Audio Assets (setup guidance)
- `SC_Crow_Caw_01/02/03` - Crow call sound cues
- `SC_Crow_WingFlap` - Wing flap sound cue
- `SC_Crow_Attenuation` - Attenuation settings (80cm inner sphere, 15m falloff)

### Materials
- `M_Crow` - Crow feather material (from Winter Hunt Animal Variety Pack)
- `M_SnowBody`, `M_Coal`, `M_Carrot`, `M_SnowGround` - Snowman materials

### Level Example
- `L_CrowSnowman` - Example level with snowman, crow on head, and trigger

---

*Plugin created for UE52VFX project, adapted for general use in other UE5.2 projects.*