// Fill out your copyright notice in the Description page of Project Settings.

#include "SAttributeComponent.h"

#include "Core/SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamagedMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


USAttributeComponent::USAttributeComponent()
{
	HealthData.MaxHealth = 100.0f;
	HealthData.Health = HealthData.MaxHealth;
	
	CanEarnCredit = false;

	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::GetIsAlive() const
{
	return !FMath::IsNearlyZero(HealthData.Health);
}

void USAttributeComponent::OnRep_HealthData(FHealthChangeData OldHealthData)
{
	const float Delta = HealthData.Health - OldHealthData.Health;
	OnHealthChanged.Broadcast(HealthData.Instigator, this, HealthData.Health, Delta);
}

void USAttributeComponent::OnRep_CreditData(FCreditChangeData OldCreditData)
{
	const int Delta = CreditData.Credit - OldCreditData.Credit;
	OnCreditChanged.Broadcast(CreditData.Instigator, this, CreditData.Credit, Delta);
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->HasAuthority()) { return false; }

	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f) { return false; }

	if (Delta < 0.0f)
	{
		Delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}

	const float OldHealth = HealthData.Health;
	HealthData.Health = FMath::Clamp(OldHealth + Delta, 0.0f, HealthData.MaxHealth);
	HealthData.Instigator = InstigatorActor;

	const float ActualDelta = HealthData.Health - OldHealth;

	if (!FMath::IsNearlyZero(ActualDelta))
	{
		OnHealthChanged.Broadcast(InstigatorActor, this, HealthData.Health, ActualDelta);
	}

	if (FMath::IsNearlyZero(HealthData.Health) && ActualDelta < 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (IsValid(GM))
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
}

bool USAttributeComponent::ApplyCreditChange(AActor* InstigatorActor, int Credit)
{
	if (!GetOwner()->HasAuthority()) {return false; }
	if (CanEarnCredit)
	{
		CreditData.Credit += Credit;
		CreditData.Instigator = InstigatorActor;

		OnCreditChanged.Broadcast(InstigatorActor, this, CreditData.Credit, Credit);

		return true;
	}
	return false;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (IsValid(FromActor))
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}
	return nullptr;
}

bool USAttributeComponent::GetIsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttrComp = GetAttributes(FromActor);
	if (AttrComp)
	{
		return AttrComp->GetIsAlive();
	}
	return false;
}


void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, HealthData);
	DOREPLIFETIME(USAttributeComponent, CreditData);
}