// Fill out your copyright notice in the Description page of Project Settings.


#include "SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "ActionSystem/SActionComponent.h"
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
	
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
		
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
	
}

APawn* ASAICharacter::GetTarget() const
{
	AAIController* AICont = Cast<AAIController>(GetController());
	if (IsValid(AICont))
	{
		APawn* Target = Cast<APawn>(AICont->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		return IsValid(Target) ? Target : nullptr;
	}
	return nullptr;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SeenPawns.AddUnique(Pawn);
	
	UpdateBestTarget();
	
	DrawDebugString(GetWorld(), GetActorLocation(), "Player Spotted!!", nullptr, FColor::Green, 4.0f, true);
}

void ASAICharacter::PlayerSpotted_Elapsed()
{
	if (IsValid(ActivePlayerSpotted))
	{
		ActivePlayerSpotted->RemoveFromViewport();
	}
}

void ASAICharacter::MulticastPlayerSpotted_Implementation()
{
	ShowPlayerSpottedWidget();
}

void ASAICharacter::ShowPlayerSpottedWidget()
{
	if (!IsValid(ActivePlayerSpotted))
	{
		ActivePlayerSpotted = CreateWidget<USWorldUserWidget>(GetWorld(), PlayerSpottedWidgetClass);
	}
	if (!GetWorldTimerManager().IsTimerActive(ActivePlayerSpottedTimerHandle))
	{
		ActivePlayerSpotted->AttachToActor = this;
		ActivePlayerSpotted->AddToViewport();
		
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "PlayerSpotted_Elapsed");
		GetWorld()->GetTimerManager().SetTimer(ActivePlayerSpottedTimerHandle, Delegate, 2.0f, false);
	}
}

void ASAICharacter::LoseTarget_Elapsed()
{
	SetTarget(nullptr);
}

void ASAICharacter::UpdateBestTarget()
{
	SeenPawns.RemoveAll([](const TObjectPtr<APawn>& MyPawn)
	{
		return !IsValid(MyPawn) || !USAttributeComponent::GetIsActorAlive(MyPawn);
	});
	
	AActor* BestTarget = nullptr;
	float BestDistSq = MAX_FLT;
	
	for (const TObjectPtr<APawn>& Pawn : SeenPawns)
	{
		const float DistSq = FVector::DistSquared(GetActorLocation(), Pawn->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Pawn;
		}
	}
	
	SetTarget(BestTarget);
}


void ASAICharacter::SetTarget(AActor* Target)
{
	AAIController* AICont = Cast<AAIController>(GetController());
	
	if (IsValid(AICont))
	{
		UBlackboardComponent* BB = AICont->GetBlackboardComponent();
		AActor* OldTarget = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
		
		BB->SetValueAsObject("TargetActor", Target);
		
		if (IsValid(Target))
		{
			if (Target != OldTarget)
			{
				MulticastPlayerSpotted();
			}
			
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "LoseTarget_Elapsed");
			GetWorld()->GetTimerManager().SetTimer(LoseTargetTimerHandle, Delegate, LoseTargetTime, false);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(LoseTargetTimerHandle);
		}
	}
}


void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		
		if (IsValid(InstigatorActor) && InstigatorActor != this)
		{
			if (APawn* InstigatorPawn = Cast<APawn>(InstigatorActor))
			{
				SeenPawns.AddUnique(InstigatorPawn);
			}
			SetTarget(InstigatorActor);
		}
		
		if (!(IsValid(ActiveHealthBar)))
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (IsValid(ActiveHealthBar))
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
			
			PawnSensingComp->bEnableSensingUpdates = false;
			GetWorldTimerManager().ClearTimer(ActivePlayerSpottedTimerHandle);
			GetWorldTimerManager().ClearTimer(LoseTargetTimerHandle);
            
			if (IsValid(ActivePlayerSpotted))
			{
				ActivePlayerSpotted->RemoveFromParent();
			}
			
			GetMesh()->SetCollisionProfileName("Ragdoll");
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
			SetLifeSpan(15.0f);
		}
	}
}