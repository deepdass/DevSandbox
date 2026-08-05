// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "Development/SCheatManager.h"

ASPlayerController::ASPlayerController()
{
	CheatClass = USCheatManager::StaticClass();
}

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	
	OnPawnChanged.Broadcast(InPawn);
}

void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();
	
	BlueprintBeginPlayingEvent();
}
