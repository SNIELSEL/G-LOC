#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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

protected:

	void SetCar(UButton* ClickedButton);

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAnyMenuButtonClicked();
	UFUNCTION()
	void OnAnyTeamSelectionButtonClicked();
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

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

	//Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

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

	//Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SelectionReturnText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SelectionConfirmText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SettingsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CreditsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuitText;

	//Image
	UPROPERTY(meta = (BindWidget))
	UImage* vanskaLore;

	UPROPERTY(meta = (BindWidget))
	UImage* SchwalbeLore;

	UPROPERTY(meta = (BindWidget))
	UImage* YamazakiLore;

	UPROPERTY(meta = (BindWidget))
	UImage* CannovaLore;

	//Button Ui mapping that in the code links the button to the UI it enables
	TMap<UButton*, UWidget*> ButtonToTargetMap;
	TMap<UButton*, UTextBlock*> ButtonToTextMap;
	TMap<UButton*, UWidget*> ButtonToLoreMap;

	UButton* CurrentHoveredButton;
	bool HoveringOnButton;
};
