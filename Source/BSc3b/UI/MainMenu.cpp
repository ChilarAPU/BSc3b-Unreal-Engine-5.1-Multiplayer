// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "GlobalHUD.h"
#include "Components/Border.h"
#include "Components/EditableText.h"
#include "Components/VerticalBox.h"

void UMainMenu::ShowServerBrowser()
{
	ServerBrowser->SetVisibility(ESlateVisibility::Visible);
	ServerBrowserHeader->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::AddSlotToServerBrowser(UUserWidget* IncomingWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot Added"));
	ServerBrowser->AddChildToVerticalBox(IncomingWidget);
}

FString UMainMenu::GetServerPassword()
{
	return FString(ServerPassword->GetText().ToString());
}

void UMainMenu::ClearServerBrowser()
{
	ServerBrowser->ClearChildren();
}

void UMainMenu::ShowUserFeedback(bool bShouldHide)
{
	UserFeedback->SetVisibility(ESlateVisibility::Visible);
	if (UserFeedbackLoadAnimation)
	{
		PlayAnimation(UserFeedbackLoadAnimation);
		if (!bShouldHide)
		{
			UGlobalHUD::SetTimerWithDelegate(UserFeedbackHandle, FTimerDelegate::CreateUObject(this, &UMainMenu::HideUserFeedback),
        			5.f, false, GetWorld());
			/* OnAnimationFinished has some major bugs that I could not address in time so this is the
			 * temporary work around
			 */
		} else
		{
			GetWorld()->GetTimerManager().ClearTimer(UserFeedbackHandle); //Clear old timer in case one is still active
		}
		
	}
}

void UMainMenu::HideUserFeedback()
{
	if (UserFeedbackHideAnimation)
	{
		PlayAnimation(UserFeedbackHideAnimation);
		//Timer delegate is run once the animation has finished
	}
}
