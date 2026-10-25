# UE52VFX Crow & Snowman Plugin

<div style="display: flex; justify-content: space-between; margin-bottom: 20px;">
  <details open style="flex-basis: 49%;">
    <summary><b>English</b></summary>
    <div style="padding: 10px; border: 1px solid #ddd; border-radius: 5px; height: 500px; overflow-y: auto;">
    
    ## Overview

    **UE52VFX_Crow** is a Unreal Engine 5.2 plugin that implements a crow sitting on a snowman, taking off when the player approaches, and flying away. The plugin provides:

    - **ACrowActor** - Crow AI with flight logic, states (Idle, Alarmed, TakingOff, Flying, Landing)
    - **ASnowmanActor** - Snowman with three activation radius zones and crow spawning
    - **ACrowTrigger** - Trigger volume that can spawn and trigger crows
    - Sound system with `USoundCue` and `USoundAttenuation` assets
    - Fully Blueprintable C++ classes with editable parameters in Details panel

    ## Features

    ### Crow Behavior
    - **Idle state**: Crow sits and optionally rotates to face the player
    - **Alarmed**: When player enters activation radius, crow prepares to take off after a delay
    - **TakingOff**: Plays takeoff animation and wing flap sound
    - **Flying**: Crow flies away from the player with configurable speed and path
    - **Landing**: Crow lands or is destroyed after maximum flight time/distance

    ### Configurable Parameters (in Blueprints)
    - Activation radius (how close player must be)
    - Trigger delay (wait time before flying)
    - Flight speed and climb height
    - Maximum flight time and despawn distance
    - Sound cues (caw, wing flap)
    - Attenuation settings (volume falloff with distance)

    ### Snowman Features
    - Three radius zones (outer, middle, inner) for different reaction distances
    - Dynamic material instances for snow body, coal eyes, carrot nose
    - Spawns crow on trigger activation
    - Configurable crow class and parameters

    ### Trigger System
    - One-shot or multi-shot triggers
    - Configurable activation radius and delay
    - Can manage existing crow actors
    - Delegates for crow state changes

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

    ### Required Dependencies
    - Unreal Engine 5.2 or later
    - No external dependencies

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

    ### Blueprint Setup Examples

    **To trigger crow flight:**
    ```blueprint
    Event BeginPlay
        -> Get ACrowTrigger
        -> Call TriggerCrow()
    ```

    **To check if crow is flying:**
    ```blueprint
    Event Tick
        -> Get ACrowTrigger
        -> Call IsCrowFlying()
        -> Branch (result)
            -> Do flying logic
    ```

    **To reset crow to idle:**
    ```blueprint
    Event AnyKey
        -> Get ASnowmanActor
        -> Call ResetState()
    ```

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

    ## Project Structure

    ```
    UE52VFX_Crow/
    ├─ Config/
    ├─ Content/           ← Blueprints, materials, sounds
    ├─ Source/UE52VFX_Crow/  ← C++ source files
    │  ├─ CrowActor.h/.cpp
    │  ├─ SnowmanActor.h/.cpp
    │  ├─ CrowTrigger.h/.cpp
    │  ├─ CrowAssetLibrary.h/.cpp
    │  ├─ CrowSoundLibrary.h/.cpp
    │  └─ UE52VFX_Crow.Build.cs
    ├─ uplugin/UE52VFX_Crow.uplugin
    └─ README.md
    ```

    ## License

    This plugin is free to use and modify. Attribution is appreciated but not required.

    ---
    *Plugin created for UE52VFX project, adapted for general use in other UE5.2 projects.*
    </div>

  <details style="flex-basis: 49%;">
    <summary><b>Русский</b></summary>
    <div style="padding: 10px; border: 1px solid #ddd; border-radius: 5px; height: 500px; overflow-y: auto;">

    ## Обзор

    **UE52VFX_Crow** — плагин для Unreal Engine 5.2, реализующий ворону, сидящую на снеговике, которая улетает когда к ней подходит игрок. Плагин предоставляет:

    - **ACrowActor** —{Crow AI} с логикой полета и состояниями
    - **ASnowmanActor** —{Snowman actor} с тремя зонами активации и спавном вороны
    - **ACrowTrigger** —{Trigger volume} который может спавнить и активировать ворон
    - Звуковая система с `USoundCue` и `USoundAttenuation`
    - Полностью blueprintable C++ классы с настраиваемыми параметрами в Details

    ## Возможности

    ### Поведение вороны
    - **Idle state**: {Idle state} Ворона сидит и optionally вращается к игроку
    - **Alarmed**: {Alarmed} Когда игрок заходит в радиус активации, ворона готовится взлететь с задержкой
    - **TakingOff**: {TakingOff} Проигрывается анимация взлета и звук взмаха крыльями
    - **Flying**: {Flying} Ворона летит away от игрока с настраиваемой скоростью
    - **Landing**: {Landing} Ворона садышится или уничтожается после макс. времени/дистанции

    ### Настраиваемые параметры (в Blueprints)
    - Радиус активации (насколько близко должен быть игрок)
    - Задержка активации (время ожидания перед полетом)
    - Скорость полета и набор высоты
    - Максимальное время полёта и дистанция сброса
    - Звуковые cue (карканье, взмах крыльями)
    - Настройки аттенюации (затухание громкости на расстоянии)

    ### Возможности снеговика
    - Три зоны перекрывания (внешний, средний, внутренний радиусы) для разных расстояний реакции
    - Динамические материалы экземпляров для тела снеговика, угольные глаза, морковная носа
    - Спавнит ворону при активации
    - Настраиваемый класс вороны и параметры

    ### Система триггеров
    - Однократный или многократный триггер
    - Настраиваемый радиус активации и задержка
    - Может управлять существующими воронами
    - Делегаты для состояний ворны

    ## Установка

    ### Способ 1: Через меню Plugins
    1. Положите папку `UE52VFX_Crow` в директорию `Plugins/` вашего проекта
    2. Включите плагин в **Edit → Plugins → UE52VFX_Crow**
    3. Перезапустите редактор Unreal Engine
    4. C++ классы автоматически пересобуруются

    ### Способ 2: Ручная настройка
    1. Перенесите `Plugins/UE52VFX_Crow/` в папку `Plugins/` вашего проекта
    2. Добавьте `UE52VFX_Crow` в `Build.cs` вашего модуля:
       ```csharp
       PublicDependencyNames.Add("UE52VFX_Crow");
       ```
    3. Пересоберите решение (Build → Generate Project Files → Build)

    ### Требуемые зависимости
    - Unreal Engine 5.2 или выше
    - Внешних зависимостей нет

    ## Использование

    ### Спавн вороны на снеговике
    1. Положите `ASnowmanActor` в уровень
    2. Установите свойство `Crow Class` в `BP_Crow` (или класс `ACrowActor` наследник)
    3. Назначьте материалы: `M_SnowBody`, `M_Coal`, `M_Carrot`, `M_SnowGround`
    4. Настройте значения радиусов: `OuterActivationRadius`, `MiddleActivationRadius`, `InnerActivationRadius`
    5. Установите `CrowTriggerDelay` для задержки перед полетом
    6. Поиграйте — когда игрок займет средний радиус, ворона взлетит

    ### Использование объемного триггера
    1. Положите `ACrowTrigger` в уровень
    2. Установите `ActivationRadius` для области триггера
    3. Установите `TriggerDelay` для задержки
    4. Выберите `bSpawnCrow` для автоматического спавна вороны
    5. Выберите `bOneShot` для однократного срабатывания или разрешите multiple activations
    6. Назначьте `CrowClass` если хотите конкретный класс ворны

    ### Примеры настройки в Blueprints

    **Для активации полета вороны:**
    ```blueprint
    Event BeginPlay
        -> Get ACrowTrigger
        -> Call TriggerCrow()
    ```

    **Для проверки полета вороны:**
    ```blueprint
    Event Tick
        -> Get ACrowTrigger
        -> Call IsCrowFlying()
        -> Branch (result)
            -> Do flying logic
    ```

    **Для возврата ворны в idle состояние:**
    ```blueprint
    Event AnyKey
        -> Get ASnowmanActor
        -> Call ResetState()
    ```

    ## Включенный контент

    ### C++ классы
    - `ACrowActor` — Основной actor поведения вороны
    - `ASnowmanActor` — Snowman с возможностью спавна вороны
    - `ACrowTrigger` — Объем триггера для активации вороны
    - `UCrowAssetLibrary` — Помощник создания ассетов
    - `UCrowSoundLibrary` — Управление звуками

    ### Blueprints (полученные от C++)
    - `BP_Crow` — Редактируемый blueprint на основе `ACrowActor`
    - `BP_Snowman` — Редактируемый blueprint на основе `ASnowmanActor`
    - `BP_CrowTrigger` — Редактируемый blueprint на основе `ACrowTrigger`

    ### Аудио-ассеты (инструкция по настройке)
    - `SC_Crow_Caw_01/02/03` — Звуковые cue карканья
    - `SC_Crow_WingFlap` — Звуковый cue взмаха крыльями
    - `SC_Crow_Attenuation` — Настройки аттенюации (80см внутренняя сфера, 15м затухание)

    ### Материалы
    - `M_Crow` — Материал перьев вороны (из Winter Hunt Animal Variety Pack)
    - `M_SnowBody`, `M_Coal`, `M_Carrot`, `M_SnowGround` — Материалы снеговика

    ### Пример уровня
    - `L_CrowSnowman` — Уровень с снеговиком, вороной на голове и триггером

    ## Структура проекта

    ```
    UE52VFX_Crow/
    ├─ Config/
    ├─ Content/           ← Blueprints, materials, sounds
    ├─ Source/UE52VFX_Crow/  ← C++ source files
    │  ├─ CrowActor.h/.cpp
    │  ├─ SnowmanActor.h/.cpp
    │  ├─ CrowTrigger.h/.cpp
    │  ├─ CrowAssetLibrary.h/.cpp
    │  ├─ CrowSoundLibrary.h/.cpp
    │  └─ UE52VFX_Crow.Build.cs
    ├─ uplugin/UE52VFX_Crow.uplugin
    └─ README.md
    ```

    ## Лицензия

    Данный плагин бесплатен для использования и модификации. Атрибуция приветствуется, но не требуется.

    ---
    *Плагин создан для проекта UE52VFX, адаптирован для общего использования в других проектах UE5.2.*
    </div>
</div>