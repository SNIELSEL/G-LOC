#include "IngameUI.h"
#include "Components/ProgressBar.h"

void UIngameUI::SetHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
}
