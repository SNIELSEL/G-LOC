#include "MainMenuUI.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "../MySaveGame.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("MainMenuUI NativeConstruct"));

	MainMenuMusicComp = UGameplayStatics::SpawnSound2D(this, MenuMusic);

	ButtonToTargetMap.Add(StartButton, TeamSelectPanel);
	ButtonToTargetMap.Add(SettingsButton, SettingsPanel);
	ButtonToTargetMap.Add(CreditsButton, CreditsPanel);
	ButtonToTargetMap.Add(SettingsBackButton, MainPanel);
	ButtonToTargetMap.Add(CreditsBackButton, MainPanel);
	ButtonToTargetMap.Add(QuitButton, MainPanel);
	ButtonToTargetMap.Add(SelectionReturn, MainPanel);
	ButtonToTargetMap.Add(SelectionConfirm, TeamSelectPanel);
	ButtonToTargetMap.Add(Sound, SoundPanel);
	ButtonToTargetMap.Add(SoundBackButton, SettingsPanel);

	ButtonToTextMap.Add(SelectionReturn, SelectionReturn);
	ButtonToTextMap.Add(SelectionConfirm, SelectionConfirm);
	ButtonToTextMap.Add(StartButton, StartButton);
	ButtonToTextMap.Add(SettingsButton, SettingsButton);
	ButtonToTextMap.Add(Video, Video);
	ButtonToTextMap.Add(Sound, Sound);
	ButtonToTextMap.Add(SoundBackButton, SoundBackButton);
	ButtonToTextMap.Add(SettingsBackButton, SettingsBackButton);
	ButtonToTextMap.Add(CreditsButton, CreditsButton);
	ButtonToTextMap.Add(CreditsBackButton, CreditsBackButton);
	ButtonToTextMap.Add(QuitButton, QuitButton);

	ButtonToLoreMap.Add(Vanska1, vanskaLore);
	ButtonToLoreMap.Add(Vanska2, vanskaLore);
	ButtonToLoreMap.Add(Schwalbe1, SchwalbeLore);
	ButtonToLoreMap.Add(Schwalbe2, SchwalbeLore);
	ButtonToLoreMap.Add(Yamazaki1, YamazakiLore);
	ButtonToLoreMap.Add(Yamazaki2, YamazakiLore);
	ButtonToLoreMap.Add(Cannova1, CannovaLore);
	ButtonToLoreMap.Add(Cannova2, CannovaLore);

	for (const TPair<UButton*, UWidget*>& Pair : ButtonToTargetMap)
	{
		if (Pair.Key)
		{
			Pair.Key->OnClicked.AddDynamic(this, &UMainMenuUI::OnAnyMenuButtonClicked);
		}
	}

	for (const TPair<UButton*, UWidget*>& Pair : ButtonToLoreMap)
	{
		if (Pair.Key)
		{
			Pair.Key->OnClicked.AddDynamic(this, &UMainMenuUI::OnAnyTeamSelectionButtonClicked);
		}
	}

	for (const TPair<UButton*, UButton*>& Pair : ButtonToTextMap)
	{
		if (Pair.Key)
		{
			Pair.Key->OnHovered.AddDynamic(this, &UMainMenuUI::OnAnyButtonHovered);
			Pair.Key->OnUnhovered.AddDynamic(this, &UMainMenuUI::OnAnyButtonUnhovered);
		}
	}

	if (UGameplayStatics::DoesSaveGameExist(TEXT("VolumeSlot"), 0))
	{
		UMySaveGame* LoadedGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("VolumeSlot"), 0));
		if (LoadedGame)
		{
			float Master = LoadedGame->MasterVolume;
			float Music = LoadedGame->MusicVolume;
			float SFX = LoadedGame->SFXVolume;

			if (MasterSlider) MasterSlider->SetValue(Master);
			if (MusicSlider)  MusicSlider->SetValue(Music);
			if (SFXSlider)    SFXSlider->SetValue(SFX);

			OnMasterVolumeChanged(Master);
			OnMusicVolumeChanged(Music);
			OnSFXVolumeChanged(SFX);
		}
	}
	else
	{
		SaveVolumeSettings(1, 1, 1);
	}

	if (MasterSlider)
	{
		MasterSlider->OnValueChanged.AddDynamic(this, &UMainMenuUI::OnMasterVolumeChanged);
	}
	if (MusicSlider)
	{
		MusicSlider->OnValueChanged.AddDynamic(this, &UMainMenuUI::OnMusicVolumeChanged);
	}
	if (SFXSlider)
	{
		SFXSlider->OnValueChanged.AddDynamic(this, &UMainMenuUI::OnSFXVolumeChanged);
	}

	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveGameInstance->SelectedCar = 0;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Slot"), 0);
}
void UMainMenuUI::OnMasterVolumeChanged(float Value)
{
	if (MainSoundMix && MasterClass)
	{
		UGameplayStatics::ClearSoundMixClassOverride(this, MainSoundMix, MusicClass);
		UGameplayStatics::SetSoundMixClassOverride(this, MainSoundMix, MasterClass, Value, 1.0f, 0.0f, true);
		UGameplayStatics::PushSoundMixModifier(this, MainSoundMix);
	}

	if (MasterProgress)
	{
		MasterProgress->SetPercent(Value);
		MasterVolume = Value;
		SaveVolumeSettings(MasterVolume, MusicVolume, SFXVolume);
	}
}

void UMainMenuUI::OnMusicVolumeChanged(float Value)
{
	if (MainSoundMix && MusicClass)
	{
		UGameplayStatics::ClearSoundMixClassOverride(this, MainSoundMix, MusicClass);
		UGameplayStatics::SetSoundMixClassOverride(this, MainSoundMix, MusicClass, Value, 1.0f, 0.00f, true);
		UGameplayStatics::PushSoundMixModifier(this, MainSoundMix);
	}

	if (MusicProgress)
	{
		MusicProgress->SetPercent(Value);
		MusicVolume = Value;
		SaveVolumeSettings(MasterVolume, MusicVolume, SFXVolume);
	}
}

void UMainMenuUI::OnSFXVolumeChanged(float Value)
{
	if (MainSoundMix && SFXClass)
	{
		UGameplayStatics::ClearSoundMixClassOverride(this, MainSoundMix, SFXClass);
		UGameplayStatics::SetSoundMixClassOverride(this, MainSoundMix, SFXClass, Value, 1.0f, 0.00f, true);
		UGameplayStatics::PushSoundMixModifier(this, MainSoundMix);
	}

	if (SFXProgress)
	{
		SFXProgress->SetPercent(Value);
		SFXVolume = Value;
		SaveVolumeSettings(MasterVolume, MusicVolume, SFXVolume);
	}
}

void UMainMenuUI::SaveVolumeSettings(float Master, float Music, float SFX)
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	SaveGameInstance->MasterVolume = Master;
	SaveGameInstance->MusicVolume = Music;
	SaveGameInstance->SFXVolume = SFX;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("VolumeSlot"), 0);
}

void UMainMenuUI::OnAnyMenuButtonClicked()
{
	UButton* Clicked = nullptr;

	for (const TPair<UButton*, UWidget*>& Pair : ButtonToTargetMap)
	{
		if (Pair.Key && Pair.Key->HasKeyboardFocus())
		{
			Clicked = Pair.Key;
			break;
		}
	}

	if (!Clicked)
	{
		return;
	}
	else if (Clicked == QuitButton)
	{
		FGenericPlatformMisc::RequestExit(false);
	}
	else if (Clicked == SelectionConfirm)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("NewTrackDesign"));
	}

	if (UWidget** Target = ButtonToTargetMap.Find(Clicked))
	{
		if (MenuSwitcher && *Target)
		{
			MenuSwitcher->SetActiveWidget(*Target);
		}
	}
}

void UMainMenuUI::OnAnyTeamSelectionButtonClicked()
{
	UButton* Clicked = nullptr;

	for (const TPair<UButton*, UWidget*>& Pair : ButtonToLoreMap)
	{
		if (Pair.Key && Pair.Key->HasKeyboardFocus())
		{
			Clicked = Pair.Key;
			break;
		}
	}

	if (!Clicked)
	{
		return;
	}

	UMainMenuUI::SetCar(Clicked);


	if (UWidget** Target = ButtonToLoreMap.Find(Clicked))
	{
		if (LoreSwitcher && *Target)
		{
			LoreSwitcher->SetActiveWidget(*Target);
		}
	}
}


void UMainMenuUI::OnAnyButtonHovered()
{
	for (const TPair<UButton*, UButton*>& Pair : ButtonToTextMap)
	{
		if (Pair.Key && Pair.Key->IsHovered())
		{
			CurrentlyHoveredButton = Pair.Key;
			Pair.Key->SetColorAndOpacity(FLinearColor(0.680757f, 0.680757f, 0.680757f, 1.0));
			break;
		}
	}
}

void UMainMenuUI::OnAnyButtonUnhovered()
{
	for (const TPair<UButton*, UButton*>& Pair : ButtonToTextMap)
	{
		if (Pair.Key && Pair.Key == CurrentlyHoveredButton)
		{
			CurrentlyHoveredButton = nullptr;
			Pair.Key->SetColorAndOpacity(FLinearColor(0.125f, 0.341f, 0.671f, 1.0f));
			break;
		}
	}
}

void UMainMenuUI::SetCar(UButton* Clicked)
{
	int32 CarIndex = 0;

	if (Clicked == Vanska1)       CarIndex = 0;
	else if (Clicked == Vanska2)  CarIndex = 1;
	else if (Clicked == Schwalbe1) CarIndex = 2;
	else if (Clicked == Schwalbe2) CarIndex = 3;
	else if (Clicked == Yamazaki1) CarIndex = 4;
	else if (Clicked == Yamazaki2) CarIndex = 5;
	else if (Clicked == Cannova1)  CarIndex = 6;
	else if (Clicked == Cannova2)  CarIndex = 7;

	if (CarIndex >= 0)
	{
		UE_LOG(LogTemp, Display, TEXT("LOGGED"));

		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
		SaveGameInstance->SelectedCar = CarIndex;
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Slot"), 0);
	}
}
