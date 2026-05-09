// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" 
#include "projectile.h"

#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class PROJECTIDK_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
    TSubclassOf<Aprojectile> primaryprojectile;
	
public:
	// Sets default values for this character's properties
	ASCharacter();

private:
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDirectionArrows = true;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float ArrowLength = 150.f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float ArrowHeadSize = 25.f;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArmComp;
	UCameraComponent* CameraComp;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

protected: // Movement	
	
	//  Var - Input
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping_Combo;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* IA_Move;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* IA_Jump;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* IA_Look;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* IA_PrimaryFire;
	
	
	// FUNCs - Input
	void func_Move(const FInputActionValue& InputValue);
	void func_Look(const FInputActionValue& InputValue);
	void func_Jump();
	void func_PrimaryFire();
};
