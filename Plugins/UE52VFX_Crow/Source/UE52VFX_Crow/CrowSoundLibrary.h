#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrowSoundLibrary.generated.h"

USTRUCT(BlueprintType)
struct FCrowSoundParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound params")
	class USoundCue* CawCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound params")
	class USoundCue* WingFlapCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound params")
	class USoundAttenuation* Attenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound params")
	float CawVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound params")
	float WingFlapVolume = 1.0f;
};

UCLASS()
class UE52VFX_CROW_API UCrowSoundLibrary : public UObject
{
	GENERATED_BODY()

public:
	UCrowSoundLibrary();

	// Sound settings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	FCrowSoundParams Params;

	// Function to play caw sound
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayCaw();

	// Function to play wing flap sound
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayWingFlap();

	// Function to stop all sounds
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void StopAllSounds();
};