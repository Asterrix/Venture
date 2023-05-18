// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PC_PlayerController.h"
#include "Projectile.h"
#include "GameFramework/FloatingPawnMovement.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	SetRootComponent(CapsuleComponent);

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerControllerRef = Cast<APlayerController>(GetController());
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerControllerRef)
	{
		FHitResult HitResult;
		PlayerControllerRef->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		// Used for debug purposes
		DrawDebugSphere(
			GetWorld(),
			HitResult.ImpactPoint,
			25.f,
			12,
			FColor::Purple,
			false,
			-1.f);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	const APC_PlayerController* PlayerController = Cast<APC_PlayerController>(Controller);
	check(EnhancedInputComponent && PlayerController);

	EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this,
	                                   &APlayerCharacter::Move);
	EnhancedInputComponent->BindAction(PlayerController->RotateAction, ETriggerEvent::Triggered, this,
	                                   &APlayerCharacter::Rotate);
	EnhancedInputComponent->BindAction(PlayerController->FireAction, ETriggerEvent::Triggered, this,
	                                   &APlayerCharacter::Fire);

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(PlayerController->PawnMappingContext, 0);
}

void APlayerCharacter::Move(const FInputActionValue& ActionValue)
{
	const FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
	const float ReverseSpeed = MoveScale / 2.f;

	// Calculate the rotation amount based on the input and rotation scale
	const float RotationAmount = Input.Y * GetWorld()->GetDeltaSeconds() * RotationSpeed;

	// Apply the rotation to the character
	const FRotator TargetRotation = GetActorRotation() + FRotator(0.f, RotationAmount, 0.f);
	SetActorRotation(TargetRotation);

	// Move in the forward or backward direction
	const float ForwardMovementAmount = FMath::Clamp(Input.X, 0.f, 1.f) * GetWorld()->GetDeltaSeconds() * MoveScale;
	const float ReverseMovementAmount = FMath::Clamp(Input.X, -1.f, 0.f) * GetWorld()->GetDeltaSeconds() * ReverseSpeed;
	const float MovementAmount = ForwardMovementAmount + ReverseMovementAmount;
	AddMovementInput(GetActorForwardVector(), MovementAmount);
}

void APlayerCharacter::Rotate(const FInputActionValue& ActionValue)
{
	constexpr float InterpolationSpeed = 5.f;

	FRotator Input(0.f, ActionValue[1], 0.f); // Only rotate around the Y-axis
	Input *= GetWorld()->GetDeltaSeconds() * RotationSpeed;

	const FRotator TargetRotation = TurretMesh->GetComponentRotation() + Input;

	const FRotator SmoothedRotation =
		FMath::RInterpTo(
			TurretMesh->GetComponentRotation(),
			TargetRotation,
			GetWorld()->GetDeltaSeconds(),
			InterpolationSpeed);

	TurretMesh->SetWorldRotation(SmoothedRotation); // Set the rotation of TurretMesh
}

void APlayerCharacter::Fire()
{
	const FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	const FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
}
