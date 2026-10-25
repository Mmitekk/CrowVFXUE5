// Copyright Epic Games, Inc. All Rights Reserved.
// Editor helper used by the setup scripts (Python / Blueprint) to build SoundCue assets.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrowAssetLibrary.generated.h"

class USoundWave;
class USoundCue;
class USoundAttenuation;

UCLASS()
class UE52VFX_API UCrowAssetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Creates (or recreates) a SoundCue asset with a single WavePlayer node and an optional attenuation.
	 * Editor only; returns nullptr in a packaged game.
	 */
	UFUNCTION(BlueprintCallable, Category = "Crow|Assets")
	static USoundCue* CreateSoundCueFromWave(USoundWave* Wave, USoundAttenuation* Attenuation, float Volume,
		const FString& PackagePath, const FString& AssetName);
};
