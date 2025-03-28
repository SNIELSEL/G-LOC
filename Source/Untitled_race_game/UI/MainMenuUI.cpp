#include "MainMenuUI.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "GenericPlatform/GenericPlatformMisc.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonToTargetMap.Add(SettingsButton, SettingsPanel);
	ButtonToTargetMap.Add(CreditsButton, CreditsPanel);
	ButtonToTargetMap.Add(SettingsBackButton, MainPanel);
	ButtonToTargetMap.Add(CreditsBackButton, MainPanel);
	ButtonToTargetMap.Add(QuitButton, MainPanel);

	for (const TPair<UButton*, UWidget*>& Pair : ButtonToTargetMap)
	{
		if (Pair.Key)
		{
			Pair.Key->OnClicked.AddDynamic(this, &UMainMenuUI::OnAnyMenuButtonClicked);
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
