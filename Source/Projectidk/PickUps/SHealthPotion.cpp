// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "Core/SPlayerState.h"
#include "PlayerComps/SAttributeComponent.h"


// Sets default values
ASHealthPotion::ASHealthPotion()
{
	
	DeactiveforTime = 10.0f;
	HealAmount = +50;
	CreditCost = -10;
	
}


void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(IsValid(InstigatorPawn)))
	{
		return;
	}
	
	USAttributeComponent* AttributeComp = InstigatorPawn->FindComponentByClass<USAttributeComponent>();
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();

	if (ensure(IsValid(AttributeComp) && IsValid(PlayerState)))
	{

		if (!((AttributeComp->GetHealth() < AttributeComp->GetMaxHealth())) || !((PlayerState->GetCredit() >= FMath::Abs(CreditCost))))
		{
			return;
		}

		if (!PlayerState->ApplyCreditChange(this, CreditCost))
		{
			return;
		}

		AttributeComp->ApplyHealthChange(this, HealAmount);

		Super::Interact_Implementation(InstigatorPawn);
	}
}

