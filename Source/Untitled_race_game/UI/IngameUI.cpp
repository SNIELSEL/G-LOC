#include "IngameUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UIngameUI::SetBoost(float CurrentBoost, float MaxBoost)
{
    if (!BoostBar || MaxBoost <= KINDA_SMALL_NUMBER)
        return;

    BoostBar->SetPercent(CurrentBoost / MaxBoost);
}

void UIngameUI::SetSpedometer(float Speed)
{
    Spedometer->SetPercent(Speed);
}

void UIngameUI::SetCurrentLaptime(FText Time)
{
    CurrentLapTimeText->SetText(Time);
}

void UIngameUI::SetFillImage(UTexture2D* Image)
{
    if (!BoostBar || !Image)
        return;

    FProgressBarStyle ProgressBarStyle = BoostBar->GetWidgetStyle();

    FSlateBrush NewFillBrush;
    NewFillBrush.SetResourceObject(Image);
    NewFillBrush.ImageSize = FVector2D(Image->GetSizeX(), Image->GetSizeY());
    NewFillBrush.DrawAs = ESlateBrushDrawType::Image;

    ProgressBarStyle.FillImage = NewFillBrush;

    BoostBar->SetWidgetStyle(ProgressBarStyle);
}
