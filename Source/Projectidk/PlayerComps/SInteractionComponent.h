// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

class USWorldUserWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTIDK_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void PrimaryInteract(); 

	// Sets default values for this component's properties
	USInteractionComponent();

protected:
	
	UFUNCTION(Server, Reliable)
	void ServerInteract();
	
	void FindBestInteractable();
	
	UPROPERTY()
	TObjectPtr<AActor> FocusedActor;
	
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceDistance;
	
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceRadius;
	
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;
	
	UPROPERTY()
	TObjectPtr<USWorldUserWidget> DefaultWidgetInst;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
