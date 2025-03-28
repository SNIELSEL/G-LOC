#include "MainMenuUI.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "GenericPlatform/GenericPlatformMisc.h"

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
	ButtonToTextMap.Add(CreditsButton, CreditsText);
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

	if (UWidget** Target = ButtonToLoreMap.Find(Clicked))
	{
		if (LoreSwitcher && *Target)
		{
			LoreSwitcher->SetActiveWidget(*Target);
		}
	}
}


void UMainMenuUI::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	FVector2D MousePos = InMouseEvent.GetScreenSpacePosition();

	for (const TPair<UButton*, UTextBlock*>& Pair : ButtonToTextMap)
	{
		if (!Pair.Key) continue;

		FGeometry ButtonGeometry = Pair.Key->GetCachedGeometry();

		if (ButtonGeometry.IsUnderLocation(MousePos) && !HoveringOnButton)
		{
			HoveringOnButton = true;
			CurrentHoveredButton = Pair.Key;
			Pair.Value->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			break;
		}
	}
}

void UMainMenuUI::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	FVector2D MousePos = InMouseEvent.GetScreenSpacePosition();

	for (const TPair<UButton*, UTextBlock*>& Pair : ButtonToTextMap)
	{
		if (!Pair.Key) continue;

		FGeometry ButtonGeometry = Pair.Key->GetCachedGeometry();

		if (!ButtonGeometry.IsUnderLocation(MousePos) && HoveringOnButton && Pair.Key == CurrentHoveredButton)
		{
			HoveringOnButton = false;
			Pair.Value->SetColorAndOpacity(FSlateColor(FLinearColor(0.125f, 0.341f, 0.671f, 1.0f)));
			break;
		}
	}

	//failsafe for when not changin color because of Switching UiPanels
	for (const TPair<UButton*, UTextBlock*>& Pair : ButtonToTextMap)
	{
		if (!Pair.Key) continue;

		if (Pair.Value->GetColorAndOpacity() == FSlateColor(FLinearColor::White))
		{
			Pair.Value->SetColorAndOpacity(FSlateColor(FLinearColor(0.125f, 0.341f, 0.671f, 1.0f)));
			break;
		}
	}
}
