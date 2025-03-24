#include "IngameUI.h"
#include "Components/ProgressBar.h"

void UIngameUI::SetBoost(float CurrentBoost, float MaxBoost)
{
    if (!BoostBar || MaxBoost <= KINDA_SMALL_NUMBER)
        return;

    BoostBar->SetPercent(CurrentBoost / MaxBoost);
}
