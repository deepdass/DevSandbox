// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class URadialForceComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTIDK_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere)
	URadialForceComponent* RadComp;
	
	// Funcs
	
	virtual void PostInitializeComponents() override; 
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
				UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
				const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) 
	void Explode();
}; 
