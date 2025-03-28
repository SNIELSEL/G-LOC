#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;

UCLASS(Abstract)
class UNTITLED_RACE_GAME_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnAnyMenuButtonClicked();

	//Switchers
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	//Panels
	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsPanel;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainPanel;

	UPROPERTY(meta = (BindWidget))
	UWidget* CreditsPanel;

	//Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsBackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsBackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	//Button Ui mapping that in the code links the button to the UI it enables
	TMap<UButton*, UWidget*> ButtonToTargetMap;
};
