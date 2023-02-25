// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBox.h"

#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"

void UChatBox::SetChatMessage(FCustomChatMessage IncomingMessage)
{
	Message->SetText(IncomingMessage.Message);
	Time->SetText(FText::FromString(IncomingMessage.TimeOfMessage));
	PlayerID->SetText(FText::FromString(IncomingMessage.PlayerID));
	
}
