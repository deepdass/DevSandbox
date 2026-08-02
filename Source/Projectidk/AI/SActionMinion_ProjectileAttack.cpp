// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionMinion_ProjectileAttack.h"

#include "ActionSystem/SActionComponent.h"
#include "AI/SAICharacter.h"
#include "GameFramework/Character.h"
#include "PlayerComps/SAttributeComponent.h"
#include "Projectiles/SBaseClassProjectile.h"


USActionMinion_ProjectileAttack::USActionMinion_ProjectileAttack()
{
	PitchMaxBulletSpread = FVector2D(0,4.0f);
	YawMaxBulletSpread = FVector2D(-5.0f,5.0f);
}

void USActionMinion_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	ASAICharacter* MyPawn = CastChecked<ASAICharacter>(GetOwningComponent()->GetOwner());

	Super::StartAction_Implementation(Instigator);

	const FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");

	AActor* TargetActor = MyPawn->GetTarget();
	check(TargetActor);

	FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
	FRotator MuzzleRotation = Direction.Rotation();

	MuzzleRotation.Pitch += FMath::RandRange(PitchMaxBulletSpread.X, PitchMaxBulletSpread.Y);
	MuzzleRotation.Yaw += FMath::RandRange(YawMaxBulletSpread.X, YawMaxBulletSpread.Y);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = MyPawn;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

	StopAction(Instigator);
}

bool USActionMinion_ProjectileAttack::CanStartAction_Implementation(AActor* Instigator)
{
	if (Super::CanStartAction_Implementation(Instigator))
	{
		ASAICharacter* MyPawn = CastChecked<ASAICharacter>(GetOwningComponent()->GetOwner());
		AActor* TargetActor = MyPawn->GetTarget();
		
		return TargetActor && USAttributeComponent::GetIsActorAlive(TargetActor);
	}

	return false;
}
