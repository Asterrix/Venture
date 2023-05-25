// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AHero::AHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 300.f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();
	
	SpringArmComponent->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if(const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CharacterMovementAction, ETriggerEvent::Triggered, this,&AHero::HandleCharacterMovementAction);
		EnhancedInputComponent->BindAction(CharacterMouseMovementAction, ETriggerEvent::Triggered, this,&AHero::HandleCharacterMouseMovementAction);
		EnhancedInputComponent->BindAction(CharacterJumpAction, ETriggerEvent::Started, this,&AHero::HandleCharacterJumpAction);
		EnhancedInputComponent->BindAction(CharacterJumpAction, ETriggerEvent::Completed, this,&AHero::HandleCharacterStopJumpAction);
	}
}

void AHero::HandleCharacterMovementAction(const FInputActionValue& ActionValue)
{
	const FVector2D MovementActionValue = ActionValue.Get<FVector2D>();
	const FRotator ControlRotation = GetControlRotation();
	const FVector RightVector = ControlRotation.Vector().RightVector;
	const FVector ForwardVector = ControlRotation.Vector().GetSafeNormal2D();
	
	AddMovementInput(RightVector, MovementActionValue.X);
	AddMovementInput(ForwardVector, MovementActionValue.Y);
}

void AHero::HandleCharacterMouseMovementAction(const FInputActionValue& ActionValue)
{
	const FVector2D MovementActionValue = ActionValue.Get<FVector2D>();
	const float MultiplierX = InvertCameraX ? -1.0f : 1.0f;
	const float MultiplierY = InvertCameraY ? -1.0f : 1.0f;

	const float ResultX = MultiplierX * MovementActionValue.X;
	const float ResultY = MultiplierY * MovementActionValue.Y;

	AddControllerYawInput(ResultX);
	AddControllerPitchInput(ResultY);
}

void AHero::HandleCharacterJumpAction()
{
	Super::Jump();
}

void AHero::HandleCharacterStopJumpAction()
{
	Super::StopJumping();
}
