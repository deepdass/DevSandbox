// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = MaxHealth;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	Health = FMath::Clamp(Health+Delta, 0.0f, MaxHealth);
		
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	return true;
	
}

bool USAttributeComponent::GetIsAlive() const
{
	return Health > 0.0f;
}


