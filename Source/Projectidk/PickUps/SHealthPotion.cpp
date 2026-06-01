// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "PlayerComps/SAttributeComponent.h"


// Sets default values
ASHealthPotion::ASHealthPotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	DeactiveforTime = 10.0f;
}


void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	
	if (ensure(IsValid(InstigatorPawn)))
	{
		return;
	}
	
	USAttributeComponent* AttributeComp = InstigatorPawn->FindComponentByClass<USAttributeComponent>();
	if (ensure(IsValid(AttributeComp)))
	{
		if (AttributeComp->GetHealth() >= AttributeComp->GetMaxHealth())
		{
			return;
		}
		AttributeComp->ApplyHealthChange(+30);
		
		Super::Interact_Implementation(InstigatorPawn);
	}
}

