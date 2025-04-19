// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IWebSocket.h"
#include "WebSocketsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LAB111_API UWebSocketsGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	// Function to allow Blueprints to set the target actor
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SetTargetActor(AActor* NewTargetActor);

	//Tracks whether the WebSocket is currently connected
	UPROPERTY(BlueprintReadOnly, Category = "WebSocket")
	bool bIsWebSocketConnected; 

	/* UFUNCTION(BlueprintCallable, Category = "WebSocket")
	bool IsWebSocketConnected() const;
 */

	// Reconnect to WebSocket (optional)
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void ReconnectWebSocket();

	// Send a message over WebSocket
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SendWebSocketMessage(const FString& Message);


private:
	// WebSocket instance
	TSharedPtr<IWebSocket> WebSocket;

	// Reference to the Blueprint actor that will receive WebSocket messages
	UPROPERTY()
	AActor* TargetActor;

	// Internal connect method
	void ConnectWebSocket();
	
};
