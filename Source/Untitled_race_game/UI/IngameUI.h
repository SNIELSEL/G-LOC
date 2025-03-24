#pragma once

#include "CoreMinimal.h"
#include"Blueprint/UserWidget.h"
#include "IngameUI.generated.h"

UCLASS(Abstract)
class UNTITLED_RACE_GAME_API UIngameUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetBoost(float CurrentBoost, float MaxBoost);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* BoostBar;
};