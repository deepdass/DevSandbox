// Fill out your copyright notice in the Description page of Project Settings.


#include "SCheatManager.h"

#include "AI/SAICharacter.h"
#include "Core/SPlayerState.h"
#include "PlayerComps/SAttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

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

	if (ACharacter* MyCharacter = MyPC->GetCharacter())
	{
		if (UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement())
		{
			MoveComp->Velocity = FVector::ZeroVector;
		}
		FVector Direction = FVector::ZeroVector;

		switch (Axis)
		{
		case EMoveAxis::X:
			Direction = MyCharacter->GetActorForwardVector();
			break;
		case EMoveAxis::Y:
			Direction = MyCharacter->GetActorRightVector();
			break;
		case EMoveAxis::Z:
			Direction = FVector::UpVector;
			break;
		}
		
		MyCharacter->AddActorWorldOffset(Direction * ByCm, false);
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

void USCheatManager::DeleteSaveGame()
{
	UGameplayStatics::DeleteGameInSlot("SaveGame01", 0);
}