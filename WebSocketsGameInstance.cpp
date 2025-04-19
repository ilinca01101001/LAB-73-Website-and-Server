#include "WebSocketsGameInstance.h"
#include "WebSocketsModule.h"
#include "Engine/Engine.h"



void UWebSocketsGameInstance::Init()
{
	Super::Init();
	bIsWebSocketConnected = false;
	ConnectWebSocket();
}

void UWebSocketsGameInstance::Shutdown()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Close();
	}
	Super::Shutdown();
}

void UWebSocketsGameInstance::ConnectWebSocket()
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	WebSocket = FWebSocketsModule::Get().CreateWebSocket(TEXT("ws://127.0.0.1:3000/ws"));

	WebSocket->OnConnected().AddLambda([this]()
		{
			bIsWebSocketConnected = true;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Successfully connected"));
		});

	WebSocket->OnConnectionError().AddLambda([this](const FString& Error)
		{
			bIsWebSocketConnected = false;  
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("WebSocket Error: ") + Error);
		});

	WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)
		{
			bIsWebSocketConnected = false;  
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, TEXT("WebSocket closed: ") + Reason);
		});

	WebSocket->OnMessage().AddLambda([this](const FString& MessageString)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, TEXT("WebSocket received: ") + MessageString);

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

	WebSocket->OnMessageSent().AddLambda([](const FString& MessageString)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("WebSocket sent: ") + MessageString);
		});

	WebSocket->Connect();
}

void UWebSocketsGameInstance::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

/* bool UWebSocketsGameInstance::IsWebSocketConnected() const
{
	return bIsWebSocketConnected;
}  */

void UWebSocketsGameInstance::ReconnectWebSocket()
{
	if (WebSocket.IsValid())
	{
		WebSocket->Close();
		WebSocket.Reset();
	}
	ConnectWebSocket();
}

void UWebSocketsGameInstance::SendWebSocketMessage(const FString& Message)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(Message);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("WebSocket not connected, message not sent."));
	}
}
