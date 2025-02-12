// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IWebSocket.h"
#include "WebSocketsGameInstance.generated.h"

/**
 * Custom GameInstance to manage WebSocket communication
 */
UCLASS()
class LAB10_API UWebSocketsGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	// Function to allow Blueprints to set the target actor
	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SetTargetActor(AActor* NewTargetActor);

private:
	// WebSocket instance
	TSharedPtr<IWebSocket> WebSocket;

	// Reference to the Blueprint actor that will receive WebSocket messages
	UPROPERTY()
	AActor* TargetActor;
};
