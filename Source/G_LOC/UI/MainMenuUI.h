#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Components/ProgressBar.h"
#include "GameFramework/SaveGame.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;

UCLASS(Abstract)
class G_LOC_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetCar(UButton* ClickedButton);

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnAnyMenuButtonClicked();
	UFUNCTION()
	void OnAnyButtonHovered();
	UFUNCTION()
	void OnAnyButtonUnhovered();
	UFUNCTION()
	void OnAnyTeamSelectionButtonClicked();
	UFUNCTION()
	void OnMasterVolumeChanged(float value);
	UFUNCTION()
	void OnMusicVolumeChanged(float value);
	UFUNCTION()
	void OnSFXVolumeChanged(float value);

	void SaveVolumeSettings(float Master, float Music, float SFX);

	//Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundMix* MainSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* MasterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* MusicClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundClass* SFXClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* MenuMusic;

	float MasterVolume;
	float MusicVolume;
	float SFXVolume;
	//Switchers
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* LoreSwitcher;

	//Panels
	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsPanel;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainPanel;

	UPROPERTY(meta = (BindWidget))
	UWidget* CreditsPanel;

	UPROPERTY(meta = (BindWidget))
	UWidget* TeamSelectPanel;

	UPROPERTY(meta = (BindWidget))
	UWidget* SoundPanel;

	//Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsBackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* Video;

	UPROPERTY(meta = (BindWidget))
	UButton* Sound;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsBackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectionReturn;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectionConfirm;

	UPROPERTY(meta = (BindWidget))
	UButton* Vanska1;

	UPROPERTY(meta = (BindWidget))
	UButton* Schwalbe1;

	UPROPERTY(meta = (BindWidget))
	UButton* Yamazaki1;

	UPROPERTY(meta = (BindWidget))
	UButton* Cannova1;

	UPROPERTY(meta = (BindWidget))
	UButton* Vanska2;

	UPROPERTY(meta = (BindWidget))
	UButton* Schwalbe2;

	UPROPERTY(meta = (BindWidget))
	UButton* Yamazaki2;

	UPROPERTY(meta = (BindWidget))
	UButton* Cannova2;

	UPROPERTY(meta = (BindWidget))
	UButton* SoundBackButton;

	//Image
	UPROPERTY(meta = (BindWidget))
	UImage* vanskaLore;

	UPROPERTY(meta = (BindWidget))
	UImage* SchwalbeLore;

	UPROPERTY(meta = (BindWidget))
	UImage* YamazakiLore;

	UPROPERTY(meta = (BindWidget))
	UImage* CannovaLore;

	//audio slider binding
	UPROPERTY(meta = (BindWidget))
	class USlider* MasterSlider;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* MasterProgress;

	UPROPERTY(meta = (BindWidget))
	class USlider* MusicSlider;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* MusicProgress;

	UPROPERTY(meta = (BindWidget))
	class USlider* SFXSlider;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SFXProgress;

	//Button Ui mapping that in the code links the button to the UI it enables
	TMap<UButton*, UWidget*> ButtonToTargetMap;
	TMap<UButton*, UButton*> ButtonToTextMap;
	TMap<UButton*, UWidget*> ButtonToLoreMap;

	UButton* CurrentlyHoveredButton = nullptr;
	UButton* CurrentHoveredButton;
	bool HoveringOnButton;
};
