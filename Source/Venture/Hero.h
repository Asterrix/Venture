// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hero.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class VENTURE_API AHero : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHero();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void HandleCharacterMovementAction(const FInputActionValue& ActionValue);
	void HandleCharacterMouseMovementAction(const FInputActionValue& ActionValue);
	void HandleCharacterJumpAction();
	void HandleCharacterStopJumpAction();

private:
	UPROPERTY()
	USpringArmComponent* SpringArmComponent;

	UPROPERTY()
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterMovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterMouseMovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterJumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool InvertCameraX;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool InvertCameraY;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	bool GetInvertCameraX() const { return InvertCameraX; };
	bool GetInvertCameraY() const { return InvertCameraY; };
};
