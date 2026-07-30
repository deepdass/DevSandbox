// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
class USActionComponent;
class SoundCue;
class UUserWidget;
class USWorldUserWidget;

UCLASS()
class PROJECTIDK_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();
	
public:
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintCallable)
	APawn* GetTarget() const;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USAttributeComponent> AttributeComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USActionComponent> ActionComp;
	
	UPROPERTY()
	TObjectPtr<USWorldUserWidget> ActiveHealthBar;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY()
	TObjectPtr<USWorldUserWidget> ActivePlayerSpotted;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> PlayerSpottedWidgetClass;
	
	FTimerHandle ActivePlayerSpottedTimerHandle;
	
	UFUNCTION()
	void PlayerSpotted_Elapsed();

	void SetTarget(AActor* Target);
	
	UFUNCTION()
	void ShowPlayerSpottedWidget();
	
	UFUNCTION(NetMulticast, UnReliable)
	void MulticastPlayerSpotted();
	
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	
	UPROPERTY()
	TArray<TObjectPtr<APawn>> SeenPawns;
	
	void UpdateBestTarget();
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float LoseTargetTime;
	
	FTimerHandle LoseTargetTimerHandle;
	
	UFUNCTION()
	void LoseTarget_Elapsed();
	
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FlashMID;
};