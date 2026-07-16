// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STestCharacter.generated.h"

class USAttributeComponent;
class USActionComponent;

UCLASS()
class PROJECTIDK_API ASTestCharacter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASTestCharacter();

protected:
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USAttributeComponent> AttributeComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USActionComponent> ActionComp;
	
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);
	
	virtual void PostInitializeComponents() override;
	
};
