#include "CarSpawner.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "../MySaveGame.h"

ACarSpawner::ACarSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

    if (UGameplayStatics::DoesSaveGameExist(TEXT("Slot"), 0))
    {
        UMySaveGame* LoadedGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Slot"), 0));
        if (LoadedGame)
        {
            SelectedCar = LoadedGame->SelectedCar;
            //UE_LOG(LogTemp, Warning, TEXT("Loaded Car Index: %d"), SelectedCar);
        }
    }
}

void ACarSpawner::BeginPlay()
{
	Super::BeginPlay();

    if (SelectedCar == 0)       CarToSpawn = Vanska33;
    else if (SelectedCar == 1)  CarToSpawn = Vanska44;
    else if (SelectedCar == 2) CarToSpawn = Schwalbe28;
    else if (SelectedCar == 3) CarToSpawn = Schwalbe37;
    else if (SelectedCar == 4) CarToSpawn = Yamazaki39;
    else if (SelectedCar == 5) CarToSpawn = Yamazaki49;
    else if (SelectedCar == 6)  CarToSpawn = Cannova56;
    else if (SelectedCar == 7)  CarToSpawn = Cannova65;


    if (CarToSpawn)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        FVector SpawnLocation = GetActorLocation() + FVector(200, 0, 0); // Adjust as needed.
        FRotator SpawnRotation = GetActorRotation();

        AMainCar* SpawnedCar = GetWorld()->SpawnActor<AMainCar>(CarToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
        if(SpawnedCar)
        {
            UE_LOG(LogTemp, Warning, TEXT("Successfully spawned a MainCar."));
        }
    }
}

