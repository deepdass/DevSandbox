// Fill out your copyright notice in the Description page of Project Settings.


#include "SCredit.h"

#include "Core/SPlayerState.h"


// Sets default values
ASCredit::ASCredit()
{
	CreditAmount = 1;
}


void ASCredit::Interact_Implementation(APawn* InstigatorPawn)
{
	AddCredit(InstigatorPawn);
}

void ASCredit::AddCredit(APawn* InstigatorPawn)
{
	if (!(IsValid(InstigatorPawn)))
	{
		return;
	}
	
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (IsValid(PlayerState))
	{
		PlayerState->ApplyCreditChange(this, CreditAmount);

		Super::Interact_Implementation(InstigatorPawn);
	}
}

