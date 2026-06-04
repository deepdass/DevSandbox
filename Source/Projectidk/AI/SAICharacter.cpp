// Fill out your copyright notice in the Description page of Project Settings.


#include "SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerComps/SAttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/SWorldUserWidget.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
	
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTarget(Pawn);
	DrawDebugString(GetWorld(), GetActorLocation(), "Player Spotted", nullptr, FColor::Green, 4.0f, true);
}


void ASAICharacter::SetTarget(AActor* Target)
{
	AAIController* AICont = Cast<AAIController>(GetController());
	
	if (AICont)
	{
		AICont->GetBlackboardComponent()->SetValueAsObject("TargetActor", Target);
		
	}
}


void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		
		if (InstigatorActor != this)
		{
			SetTarget(InstigatorActor);
		}
		
		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachToActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
		
		if (FlashMID)
		{
			FlashMID->SetScalarParameterValue(FName("TimeToHit"), GetWorld()->GetTimeSeconds());
		}
		
		if (NewHealth <= 0.0f)
		{
			AAIController* AICont = Cast<AAIController>(GetController());
			if (IsValid(AICont))
			{
				AICont->GetBrainComponent()->StopLogic("Dead");
			}
			
			GetMesh()->SetCollisionProfileName("Ragdoll");
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
			SetLifeSpan(10.0f);
		}
	}
}


