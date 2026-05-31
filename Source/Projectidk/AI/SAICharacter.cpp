// Fill out your copyright notice in the Description page of Project Settings.


#include "SAICharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AICont = Cast<AAIController>(GetController());
	
	if (AICont)
	{
		UBlackboardComponent* BlackboardComp = AICont->GetBlackboardComponent();
		
		BlackboardComp->SetValueAsObject("TargetActor", Pawn);
		
		DrawDebugString(GetWorld(), GetActorLocation(), "Player Spotted", nullptr, FColor::Green, 4.0f, true);
	}
}


