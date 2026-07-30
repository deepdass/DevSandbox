// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionSkill.h"

#include "Projectiles/SBaseClassProjectile.h"
#include "GameFramework/Character.h"
#include "PlayerComps/SAttributeComponent.h"


USActionSkill::USActionSkill()
{
	AttackAnimDelay = 0.24f;
	TimeBetweenAttacks = 0.6f;
	NextAttackAllowedTime = 0.0f;
	
	ActionCost = 20.0f;
}

void USActionSkill::SetPrimaryProjectile(TSubclassOf<ASBaseClassProjectile> NewProjectile)
{
	ProjectileClass = NewProjectile;
}

void USActionSkill::StartAction_Implementation(AActor* Instigator)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < NextAttackAllowedTime)
	{
		return;
	}

	Super::StartAction_Implementation(Instigator);
	
	ACharacter* Character = Cast<ACharacter>(Instigator);
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Character);
	if (Character && AttributeComp)
	{
		if (AttributeComp->GetRage() < ActionCost)
		{
			StopAction(Character);
			return;
		}
		NextAttackAllowedTime = CurrentTime + TimeBetweenAttacks;

		Character->PlayAnimMontage(AttackAnim);
		AttributeComp->ApplyRageChange(Character, -ActionCost);
		
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
	}
}

void USActionSkill::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		APlayerController* PC = Cast<APlayerController>(InstigatorCharacter->GetController());
		if (!PC) return;

		FVector CamStart;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamStart, CamRot);

		const FVector TraceEnd = CamStart + (CamRot.Vector() * 5000.0f);

		FHitResult HitResult;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CamStart, TraceEnd, ECC_Visibility, Params);

		FVector HandLoc = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);
		FRotator ProjectileRot = CamRot;
		if (bHit)
		{
			FVector ImpactPt = HitResult.ImpactPoint;
			ProjectileRot = (ImpactPt - HandLoc).GetSafeNormal().Rotation();
		}

		FTransform SpawnTM = FTransform(ProjectileRot, HandLoc);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;
		
		GetWorld()->SpawnActor<ASBaseClassProjectile>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}