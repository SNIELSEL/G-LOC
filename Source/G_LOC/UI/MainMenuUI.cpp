#include "MainMenuUI.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "../MySaveGame.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonToTargetMap.Add(StartButton, TeamSelectPanel);
	ButtonToTargetMap.Add(SettingsButton, SettingsPanel);
	ButtonToTargetMap.Add(CreditsButton, CreditsPanel);
	ButtonToTargetMap.Add(SettingsBackButton, MainPanel);
	ButtonToTargetMap.Add(CreditsBackButton, MainPanel);
	ButtonToTargetMap.Add(QuitButton, MainPanel);
	ButtonToTargetMap.Add(SelectionReturn, MainPanel);
	ButtonToTargetMap.Add(SelectionConfirm, TeamSelectPanel);

	ButtonToTextMap.Add(SelectionReturn, SelectionReturnText);
	ButtonToTextMap.Add(SelectionConfirm, SelectionConfirmText);
	ButtonToTextMap.Add(StartButton, StartText);
	ButtonToTextMap.Add(SettingsButton, SettingsText);
	ButtonToTextMap.Add(Video, VideoText);
	ButtonToTextMap.Add(Sound, SoundText);
	ButtonToTextMap.Add(SettingsBackButton, SettingsBackText);
	ButtonToTextMap.Add(CreditsButton, CreditsText);
	ButtonToTextMap.Add(CreditsBackButton, CreditsReturnText);
	ButtonToTextMap.Add(QuitButton, QuitText);

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

	for (const TPair<UButton*, UWidget*>& Pair : ButtonToTextMap)
	{
		if (Pair.Key)
		{
			Pair.Key->OnHovered.AddDynamic(this, &UMainMenuUI::OnAnyButtonHovered);
			Pair.Key->OnUnhovered.AddDynamic(this, &UMainMenuUI::OnAnyButtonUnhovered);
		}
	}

	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveGameInstance->SelectedCar = 0;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Slot"), 0);
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
	for (const TPair<UButton*, UWidget*>& Pair : ButtonToTextMap)
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
	for (const TPair<UButton*, UWidget*>& Pair : ButtonToTextMap)
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
