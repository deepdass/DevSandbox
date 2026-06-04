// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(AttachToActor))
	{
		RemoveFromParent();
		return;
	}
	
	FVector2D ScreenPos;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachToActor->GetActorLocation() + WorldOffset, ScreenPos))
	{
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		
		ScreenPos /= Scale;
		
		if (ensure(ParentSizeBox))
		{
			ParentSizeBox->SetRenderTranslation(ScreenPos);
		}
	}
}
