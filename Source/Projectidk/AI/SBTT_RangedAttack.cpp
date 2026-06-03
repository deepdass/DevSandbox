// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTT_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "PlayerComps/SAttributeComponent.h"


USBTT_RangedAttack::USBTT_RangedAttack()
{
	PitchMaxBulletSpread = FVector2D(0,3.0f);
	YawMaxBulletSpread = FVector2D(-3.0f,3.0f);
	
}


EBTNodeResult::Type USBTT_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	AAIController* MyAIController = OwnerComp.GetAIOwner();
	
	if (ensure(IsValid(MyAIController)))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyAIController->GetPawn());
		
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");
		
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		
		if (TargetActor == nullptr or !USAttributeComponent::GetIsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}
		
		FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();
		
		MuzzleRotation.Pitch += FMath::RandRange(PitchMaxBulletSpread.X, PitchMaxBulletSpread.Y);
		MuzzleRotation.Yaw += FMath::RandRange(YawMaxBulletSpread.X, YawMaxBulletSpread.Y);
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;
		
		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);
		
		return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
		
	}
	
	return EBTNodeResult::Failed;
}


