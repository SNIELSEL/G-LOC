#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

UCLASS()
class G_LOC_API UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category = "SaveData")
    int32 SelectedCar;

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    float MasterVolume = 1.0f;

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    float MusicVolume = 1.0f;

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    float SFXVolume = 1.0f;
};
