#pragma once

namespace Variables
{
	UEngine* Engine = nullptr;
	UWorld* World = nullptr;
	APawn* AcknowledgedPawn = nullptr;
	ULocalPlayer* LocalPlayer = nullptr;
	UGameInstance* GameInstance = nullptr;
	APlayerController* PlayerController = nullptr;
	UGameViewportClient* ViewportClient = nullptr;
	APlayerCameraManager* PlayerCameraManager = nullptr;
	AMarvelBaseCharacter* TargetPlayerPTR = nullptr;
	FVector CameraLocation = FVector();
	FRotator CameraRotation = FRotator();
	float FieldOfView = 0.f;

	FVector2D ScreenSize = FVector2D(1920, 1080);
	FVector2D ScreenCenter = FVector2D(1920 / 2, 1080 / 2);
}