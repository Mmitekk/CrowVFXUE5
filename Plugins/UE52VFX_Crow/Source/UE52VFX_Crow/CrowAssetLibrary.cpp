#include "CrowAssetLibrary.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"
#include "Kismet/KismetMathLibrary.h"

UCrowSoundLibrary* UCrowAssetLibrary::CreateCrowSoundLibrary(class USoundAttenuation* Attenuation, class USoundCue* Caw, class USoundCue* WingFlap)
{
	if (!Attenuation)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCrowAssetLibrary::CreateCrowSoundLibrary - No attenuation provided"));
		return nullptr;
	}

	UCrowSoundLibrary* Library = NewObject<UCrowSoundLibrary>();
	if (!Library)
		return nullptr;

	Library->AttenuationSettings = Attenuation;
	if (Caw)
		Library->CawSoundCue = Caw;
	if (WingFlap)
		Library->WingFlapSoundCue = WingFlap;

	return Library;
}

void UCrowAssetLibrary::SetupCrowAudio(UCrowSoundLibrary* SoundLibrary, UAudioComponent* AudioComp)
{
	if (!IsValid(SoundLibrary) || !IsValid(AudioComp))
		return;

	// Apply attenuation settings
	if (IsValid(SoundLibrary->AttenuationSettings))
	{
		AudioComp->SetSoundAttenuation(SoundLibrary->AttenuationSettings);
	}

	// Set default volumes
	float CawVol = FMath::Clamp(SoundLibrary->CawVolume, 0.0f, 1.0f);
	float WingVol = FMath::Clamp(SoundLibrary->WingFlapVolume, 0.0f, 1.0f);

	// We'll set these when playing specific cues
	// This is just a setup function
}

FCrowSoundSettings UCrowAssetLibrary::GetDefaultSoundSettings()
{
	FCrowSoundSettings Settings;
	CawVolume = 1.0f;
	WingFlapVolume = 1.0f;
	return Settings;
}