// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;


/**
 * 
 */
UCLASS()
class PROJECTIDK_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
	UPROPERTY(EditAnywhere, Category="UI")
	FVector WorldOffset;
	
public:
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(ExposeOnSpawn=true))
	TObjectPtr<AActor> AttachToActor;
	
};
