// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "Core/SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamagedMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;
	
	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::GetIsAlive() const
{
	return !FMath::IsNearlyZero(Health);
}

void USAttributeComponent::MulticastHealthChange_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f){return false;}
	
	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		
		Delta *= DamageMultiplier;
	}
	
	float OldHealth = Health;
	
	Health = FMath::Clamp(Health+Delta, 0.0f, MaxHealth);
	
	float ActualDelta = Health - OldHealth;
	if (!FMath::IsNearlyZero(ActualDelta))
	{
		MulticastHealthChange(InstigatorActor, Health, ActualDelta);
	}
	
	if (ActualDelta <= 0.0f && FMath::IsNearlyZero(Health))
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (IsValid(GM))
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}
	
	return !FMath::IsNearlyZero(ActualDelta);
	
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
	
	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);

	//DOREPLIFETIME_CONDITION(USAttributeComponent, MaxHealth, COND_OwnerOnly)
}




