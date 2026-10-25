// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowAssetLibrary.h"

#if WITH_EDITOR
#include "AssetRegistry/AssetRegistryModule.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundNodeWavePlayer.h"
#include "Sound/SoundWave.h"
#include "UObject/Package.h"
#endif

USoundCue* UCrowAssetLibrary::CreateSoundCueFromWave(USoundWave* Wave, USoundAttenuation* Attenuation, float Volume,
	const FString& PackagePath, const FString& AssetName)
{
#if WITH_EDITOR
	if (Wave == nullptr || AssetName.IsEmpty())
	{
		return nullptr;
	}

	const FString FullPath = FString::Printf(TEXT("%s/%s"), *PackagePath, *AssetName);

	UPackage* Package = CreatePackage(*FullPath);
	if (Package == nullptr)
	{
		return nullptr;
	}

	USoundCue* Cue = NewObject<USoundCue>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
	if (Cue == nullptr)
	{
		return nullptr;
	}

	USoundNodeWavePlayer* WavePlayer = NewObject<USoundNodeWavePlayer>(Cue, TEXT("WavePlayer"), RF_Transactional);
	WavePlayer->SetSoundWave(Wave);

	Cue->FirstNode = WavePlayer;
	Cue->VolumeMultiplier = Volume;
	if (Attenuation != nullptr)
	{
		Cue->bOverrideAttenuation = false;
		Cue->AttenuationSettings = Attenuation;
	}

	Cue->PostEditChange();
	Package->MarkPackageDirty();

	FAssetRegistryModule::AssetCreated(Cue);
	return Cue;
#else
	return nullptr;
#endif
}
