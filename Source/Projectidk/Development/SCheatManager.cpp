// Fill out your copyright notice in the Description page of Project Settings.


#include "SCheatManager.h"

#include "AI/SAICharacter.h"
#include "Core/SPlayerState.h"
#include "PlayerComps/SAttributeComponent.h"
#include "EngineUtils.h"

void USCheatManager::HealSelf(float Amount)
{
	
	APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(MyPawn);
		
		if (Amount < 0)
		{
			Amount = AttributeComp->GetMaxHealth();
		}
		AttributeComp->ApplyHealthChange(MyPawn, Amount);
	}
}

void USCheatManager::DamageSelf(float Amount)
{
	APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(MyPawn);
		
		if (Amount < 0)
		{
			Amount = AttributeComp->GetMaxHealth();
		}
		AttributeComp->ApplyHealthChange(MyPawn, -Amount);
	}
}

void USCheatManager::GrantCoin(int32 Amount)
{
	APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		ASPlayerState* SPlayerState = MyPawn->GetPlayerState<ASPlayerState>();
		if (IsValid(SPlayerState))
		{
			// Amount = 2147483647;  // Max int32
			SPlayerState->ApplyCreditChange(MyPawn, Amount);
		}
	}
}

void USCheatManager::MoveInDirectionBy(float ByCm, ::EMoveAxis Axis)
{
	APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		FVector Direction = FVector::ZeroVector;

		switch (Axis)
		{
		case EMoveAxis::X:
			Direction = MyPawn->GetActorForwardVector();
			break;
		case EMoveAxis::Y:
			Direction = MyPawn->GetActorRightVector();
			break;
		case EMoveAxis::Z:
			Direction = FVector::UpVector;
			break;
		}
		
		MyPawn->AddActorWorldOffset(Direction * ByCm, false);
	}
}

void USCheatManager::KillAll()
{
	APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It) 
		{
			ASAICharacter* Bot = *It;
			USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
			if (AttributeComp && AttributeComp->GetIsAlive())
			{
				AttributeComp->Kill(MyPawn);
			}
		}
	}
}