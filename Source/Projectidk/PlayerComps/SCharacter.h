// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" 
#include "Projectiles/SBaseClassProjectile.h"


#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class USAttributeComponent;
class USActionComponent;

class UInputAction;
class UInputMappingContext;
class UAnimMontage;
class UAudioComponent;


UCLASS()
class PROJECTIDK_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<ASBaseClassProjectile> primaryprojectile;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;
	
	FTimerHandle TimerHandle_PrimaryAttack;
	
	UPROPERTY(VisibleAnywhere, Category="Attack")
	FName HandSocketName = "Muzzle_01";
	
	
public:
	// Sets default values for this character's properties
	ASCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

private:
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDirectionArrows = true;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float ArrowLength = 150.f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float ArrowHeadSize = 25.f;

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USInteractionComponent* InteractionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* AttributeComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USActionComponent> ActionComp;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void SetPrimaryProjectile(TSubclassOf<ASBaseClassProjectile> projectile);
	

protected: // Movement	
	
	//  Var - Input
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputMappingContext* InputMapping_Combo;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputMappingContext* InputMapping_Interaction;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Move;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Jump;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Look;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Sprint;
	
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_PrimaryFire;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_OpenChest;
	
	
	// FUNCs - Input
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();
	
	void SprintStart();
	void SprintStop();
	
	void PrimaryFire();
	void PrimaryAttack_TimeElapsed();
	
	void OpenChest();
	
	
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);
	
	UPROPERTY(EditAnywhere, Category="Projectile")
	UAudioComponent* HealPotionSound;
	
	UPROPERTY()
	UMaterialInstanceDynamic* FlashMID;
	
public:
	virtual void PostInitializeComponents() override;
	
};
