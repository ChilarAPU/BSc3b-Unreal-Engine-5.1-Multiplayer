// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSlot.h"

#include "Components/TextBlock.h"

void UServerSlot::SetServerName(FString IncomingName)
{
	ServerName->SetText(FText::FromString(IncomingName));
}
