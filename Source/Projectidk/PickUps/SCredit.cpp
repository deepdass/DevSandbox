// Fill out your copyright notice in the Description page of Project Settings.


#include "SCredit.h"

#include "Core/SPlayerState.h"


// Sets default values
ASCredit::ASCredit()
{
	CreditAmount = 1;
}

// Called when the game starts or when spawned
void ASCredit::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCredit::Interact_Implementation(APawn* InstigatorPawn)
{
	AddCredit(InstigatorPawn);
}

// Called every frame
void ASCredit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASCredit::AddCredit(APawn* InstigatorPawn)
{
	if (!ensure(IsValid(InstigatorPawn)))
	{
		return;
	}
	
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (ensure(IsValid(PlayerState)))
	{
		PlayerState->ApplyCreditChange(this, CreditAmount);

		Super::Interact_Implementation(InstigatorPawn);
	}
}

