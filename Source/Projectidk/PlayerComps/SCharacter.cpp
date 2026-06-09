// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComps/SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "PlayerComps/SAttributeComponent.h"
#include "PlayerComps/SInteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"

#include "DrawDebugHelpers.h"
#include "ActionSystem/SActionComponent.h"
#include "ActionSystem/SAction_ProjectileAttack.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	
	HealPotionSound = CreateDefaultSubobject<UAudioComponent>("HealPotionSound");
	HealPotionSound->SetupAttachment(RootComponent);
	HealPotionSound->bAutoActivate = false;
	
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("Interaction");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	
	bUseControllerRotationYaw = false;
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay(); 
	
	FlashMID = UMaterialInstanceDynamic::Create(GetMesh()->GetOverlayMaterial(), this);
	GetMesh()->SetOverlayMaterial(FlashMID);
	
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	if (!bShowDirectionArrows) return;

	FVector Origin = GetActorLocation();

	// --- Arrow 1: Movement direction (velocity) ---
	FVector MoveDir = GetVelocity().GetSafeNormal();
	if (!MoveDir.IsNearlyZero())
	{
		DrawDebugDirectionalArrow(GetWorld(),Origin,Origin + MoveDir * ArrowLength,ArrowHeadSize,FColor::Green,false,-1.f,0,3.f);
	}

	// --- Arrow 2: Controller/look direction ---
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector LookDir = PC->PlayerCameraManager->GetCameraRotation().Vector();

		DrawDebugDirectionalArrow(GetWorld(),Origin + FVector(0.f, 0.f, 40.f), Origin + FVector(0.f, 0.f, 40.f) + LookDir * ArrowLength,ArrowHeadSize,FColor::Blue,false, -1.f, 0, 3.f);
	}
#endif
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Add the input mapping 
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) 
	{
		// gets local player subsystem
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// add input context
			LocalPlayerSubsystem->AddMappingContext(InputMapping, 0);
			LocalPlayerSubsystem->AddMappingContext(InputMapping_Combo, 0);
			LocalPlayerSubsystem->AddMappingContext(InputMapping_Interaction,0);
		}
	}
	
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASCharacter::Move);
		Input->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ASCharacter::Jump);
		Input->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ASCharacter::Look);
		
		Input->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ASCharacter::SprintStart);
		Input->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASCharacter::SprintStop);
		
		Input->BindAction(IA_PrimaryFire, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryFire);
		
		Input->BindAction(IA_OpenChest, ETriggerEvent::Triggered, this, &ASCharacter::OpenChest);
	}
}

void ASCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;
	
	FVector RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);
	
	if (IsValid(Controller))
	{
		AddMovementInput(ControlRotation.Vector(), InputVector.Y);
		AddMovementInput(RightVector, InputVector.X);
	}
}

void ASCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void ASCharacter::Jump()
{
	ACharacter::Jump();
}

void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::PrimaryFire()
{
	ActionComp->StartActionByName(this, "PrimaryFire");
}

void ASCharacter::OpenChest()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		if (FlashMID)
		{
			FlashMID->SetScalarParameterValue(FName("TimeToHit"), GetWorld()->GetTimeSeconds());
		}
		
		if (NewHealth <= 0.0f)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			DisableInput(PC);
		}
	}
	
	if (Delta > 0.0f) // add check actor later if more healing actors
	{
		HealPotionSound->Play();
	}
	
}

void ASCharacter::SetPrimaryProjectile(TSubclassOf<ASBaseClassProjectile> projectile)
{
	if (USAction_ProjectileAttack* Action = Cast<USAction_ProjectileAttack>(ActionComp->GetActionByName("PrimaryFire")))
	{
		Action->SetPrimaryProjectile(projectile);
	}
}

void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}
