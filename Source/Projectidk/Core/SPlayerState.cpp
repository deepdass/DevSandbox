// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"

#include "SaveSystem/SSaveGame.h"
#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	CanEarnCredit = true;
}

bool ASPlayerState::ApplyCreditChange(AActor* InstigatorActor, int32 Credit)
{
	if (!HasAuthority()) { return false; }

	if (CanEarnCredit)
	{
		CreditData.Credit += Credit;
		CreditData.Instigator = InstigatorActor;

		OnCreditChanged.Broadcast(InstigatorActor, this, CreditData.Credit, Credit);

		return true;
	}
	return false;
}

void ASPlayerState::OnRep_CreditData(FCreditData OldCreditData)
{
	const int32 Delta = CreditData.Credit - OldCreditData.Credit;
	OnCreditChanged.Broadcast(CreditData.Instigator, this, CreditData.Credit, Delta);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, CreditData);
}


void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credit = CreditData.Credit;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		ApplyCreditChange(GetPawn(), SaveObject->Credit);
	}
}