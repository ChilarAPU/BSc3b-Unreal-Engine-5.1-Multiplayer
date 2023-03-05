// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSlot.h"

#include "Components/TextBlock.h"

void UServerSlot::SetServerName(FString IncomingName)
{
	TArray<FStringFormatArg> args;
	args.Add(TEXT("'s Server"));
	args.Add(IncomingName);
	FString SessionName = FString::Format(TEXT("{1}{0}"), args);
	ServerName->SetText(FText::FromString(SessionName));
}

void UServerSlot::SetServerPing(int32 IncomingPing)
{
	TArray<FStringFormatArg> args;
	args.Add(IncomingPing);
	args.Add(TEXT("ms"));
	FString SessionName = FString::Format(TEXT("{0}{1}"), args);
	Ping->SetText(FText::FromString(SessionName));
}

void UServerSlot::SetTotalCurrentPlayersInServer(int32 IncomingNumber, int32 MaxIncomingNumber)
{
	TArray<FStringFormatArg> args;
	args.Add(IncomingNumber);
	args.Add(MaxIncomingNumber);
	FString SessionName = FString::Format(TEXT("{0}/{1}"), args);
	CurrentPlayerNumber->SetText(FText::FromString(SessionName));
}
