#pragma once

#include "CoreMinimal.h"
#include"Blueprint/UserWidget.h"
#include "IngameUI.generated.h"

UCLASS(Abstract)
class G_LOC_API UIngameUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetBoost(float CurrentBoost, float MaxBoost);
	
	void SetSpedometer(float Speed);

	void SetCurrentLaptime(FText Time);

	void SetFillImage(UTexture2D* Image);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* BoostBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* Spedometer;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* CurrentLapTimeText;
};