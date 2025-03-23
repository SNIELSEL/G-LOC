#pragma once

#include "CoreMinimal.h"
#include"Blueprint/UserWidget.h"
#include "IngameUI.generated.h"

UCLASS(Abstract)
class UNTITLED_RACE_GAME_API UIngameUI : public UUserWidget
{
	GENERATED_BODY()
public:
	/** Update HUD with current health. */
	void SetHealth(float CurrentHealth, float MaxHealth);

	/** Widget to use to display current health. */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;
};