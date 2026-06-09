// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBaseClassProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem; 
class UNiagaraComponent;

UCLASS(ABSTRACT)
class PROJECTIDK_API ASBaseClassProjectile : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	AActor* HitActor;
	FHitResult SweepResult;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UNiagaraSystem> ImpactVFX;
	
	UPROPERTY(BlueprintReadOnly ,EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components") 
	TObjectPtr<UNiagaraComponent> EffectComp;
	
	
	UFUNCTION()
    void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();
	
	virtual void PostInitializeComponents() override;
	
public:
	
	ASBaseClassProjectile();
	
};
