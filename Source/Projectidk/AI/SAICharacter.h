// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
class SoundCue;

UCLASS()
class PROJECTIDK_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();
	
public:
	virtual void PostInitializeComponents() override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* AttributeComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;
	
	void SetTarget(AActor* Target);
	
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	
	
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);
	
	UPROPERTY()
	UMaterialInstanceDynamic* FlashMID;
};
