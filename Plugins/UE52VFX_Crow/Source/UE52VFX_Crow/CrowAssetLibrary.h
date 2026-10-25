#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrowAssetLibrary.generated.h"

USTRUCT(BlueprintType)
struct FCrowSoundSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* CawSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* WingFlapSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float CawVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	float WingFlapVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundAttenuation* AttenuationSettings;
};

UCLASS()
class UE52VFX_CROW_API UCrowAssetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Crow|Assets")
	static UCrowSoundLibrary* CreateCrowSoundLibrary(class USoundAttenuation* Attenuation, class USoundCue* Caw, class USoundCue* WingFlap);

	UFUNCTION(BlueprintCallable, Category = "Crow|Assets")
	static void SetupCrowAudio(UCrowSoundLibrary* SoundLibrary, UAudioComponent* AudioComp);

	UFUNCTION(BlueprintCallable, Category = "Crow|Assets")
	static FCrowSoundSettings GetDefaultSoundSettings();
};