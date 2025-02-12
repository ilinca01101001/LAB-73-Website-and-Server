// Fill out your copyright notice in the Description page of Project Settings.

#include "WebSocketsGameInstance.h"
#include "WebSocketsModule.h"
#include "Engine/Engine.h"

void UWebSocketsGameInstance::Init()
{
	Super::Init();

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://127.0.0.1:3000/ws");

	// Check connection
	WebSocket->OnConnected().AddLambda([]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Successfully connected"));
		});

	WebSocket->OnConnectionError().AddLambda([](const FString& Error)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
		});

	WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, TEXT("Connection closed: ") + Reason);
		});

	// Receive message and notify the Blueprint actor
	WebSocket->OnMessage().AddLambda([this](const FString& MessageString)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, TEXT("Received message: ") + MessageString);

			// Check if the target actor is valid and call the Blueprint event
			if (TargetActor)
			{
				UFunction* Function = TargetActor->FindFunction(FName("OnWebSocketMessageReceived"));
				if (Function)
				{
					struct FWebSocketMessageParams
					{
						FString Message;
					};

					FWebSocketMessageParams Params;
					Params.Message = MessageString;
					TargetActor->ProcessEvent(Function, &Params);
				}
			}
		});

	// Send message debug log
	WebSocket->OnMessageSent().AddLambda([](const FString& MessageString)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Sent message: ") + MessageString);
		});

	WebSocket->Connect();
}

void UWebSocketsGameInstance::Shutdown()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Close();
	}
	Super::Shutdown();
}

// Function to set the target actor from Blueprints
void UWebSocketsGameInstance::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}
