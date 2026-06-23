// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerComps/SGameplayInterface.h"
#include "SBaseClass_PickUps.generated.h"

class UStaticMesh;
class UNiagaraComponent;

UCLASS()
class PROJECTIDK_API ASBaseClass_PickUps : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASBaseClass_PickUps();

protected:
	
	void Interact_Implementation(APawn* InstigatorPawn);
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components") 
	TObjectPtr<UNiagaraComponent> EffectComp;
	
	UPROPERTY()
	float DeactiveforTime;
	
	UPROPERTY(ReplicatedUsing = "OnRep_IsActive")
	bool bIsActive = true;

	UFUNCTION()
	void OnRep_IsActive();
	
	UFUNCTION()
	void Activate();
	
};