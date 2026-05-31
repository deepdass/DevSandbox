// Fill out your copyright notice in the Description page of Project Settings.


#include "SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();
	
	RunBehaviorTree(BehaviorTree);
	
	
	// APawn* PlayerCharacter = UGameplayStatics::GetPlayerPawn(this, 0);
	// if (PlayerCharacter)
	// {
	// 	GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerCharacter);
	// 	
	// }
}
