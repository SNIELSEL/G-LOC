#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainCar.h"
#include "CarSpawner.generated.h"

UCLASS()
class G_LOC_API ACarSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACarSpawner();

protected:
	virtual void BeginPlay() override;

	TSubclassOf<AMainCar> CarToSpawn;

public:	
	
	int32 SelectedCar;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Vanska33;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Vanska44;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Schwalbe28;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Schwalbe37;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Yamazaki39;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar>Yamazaki49;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Cannova56;

	UPROPERTY(EditAnywhere, Category = "Cars")
	TSubclassOf<AMainCar> Cannova65;
};
