// Fill out your copyright notice in the Description page of Project Settings.

#include "SAttributeComponent.h"

#include "Core/SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamagedMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


USAttributeComponent::USAttributeComponent()
{
	HealthData.MaxValue = 100.0f;
	HealthData.Value = HealthData.MaxValue;

	RageData.MaxValue = 100.0f;
	RageData.Value = 0.0f;

	SetIsReplicatedByDefault(true);
}

void USAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	HealthData.Value = HealthData.MaxValue;
}

bool USAttributeComponent::GetIsAlive() const
{
	return !FMath::IsNearlyZero(HealthData.Value);
}

void USAttributeComponent::OnRep_HealthData(FAttribute OldHealthData)
{
	const float Delta = HealthData.Value - OldHealthData.Value;
	OnHealthChanged.Broadcast(HealthData.Instigator, this, HealthData.Value, Delta);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->HasAuthority()) { return false; }

	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f) { return false; }

	if (Delta < 0.0f)
	{
		Delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}

	const float OldHealth = HealthData.Value;
	HealthData.Value = FMath::Clamp(OldHealth + Delta, 0.0f, HealthData.MaxValue);
	HealthData.Instigator = InstigatorActor;

	const float ActualDelta = HealthData.Value - OldHealth;

	if (!FMath::IsNearlyZero(ActualDelta))
	{
		OnHealthChanged.Broadcast(InstigatorActor, this, HealthData.Value, ActualDelta);
	}

	if (FMath::IsNearlyZero(HealthData.Value) && ActualDelta < 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (IsValid(GM))
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
}

void USAttributeComponent::OnRep_RageData(FAttribute OldRageData)
{
	const float Delta = RageData.Value - OldRageData.Value;
	OnRageChanged.Broadcast(RageData.Instigator, this, RageData.Value, Delta);
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->HasAuthority()) { return false; }

	RageData.Value = FMath::Clamp(RageData.Value + Delta, 0.0f, RageData.MaxValue);
	RageData.Instigator = InstigatorActor;

	OnRageChanged.Broadcast(InstigatorActor, this, RageData.Value, Delta);

	return !FMath::IsNearlyZero(Delta);
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
	DOREPLIFETIME(USAttributeComponent, RageData);
}


bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}