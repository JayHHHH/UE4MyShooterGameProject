// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include"Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
#include"Engine/SkeletalMeshSocket.h"
#include"DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"


AShooterCharacter::AShooterCharacter():

	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	
	PrimaryActorTick.bCanEverTick = true;


	//����һ�����ҡ��
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);//��ҡ�������ڸ������
	CameraBoom->TargetArmLength = 300.f;//����������������ľ���
	CameraBoom->bUsePawnControlRotation = true; //��ҿ��������ƶ���ת���
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	//��������˳Ƶ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//���������ҡ��ĩ��
	FollowCamera->bUsePawnControlRotation = false;//�����˳��������Ҫ����������ת

	//��Ҫ����������������ת,ֻ��ת�������
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement=false;//����ƫ�Ʋ��ٿ������﷽��
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//��ת�ٶ�
	GetCharacterMovement()->JumpZVelocity = 600.f;//��Ծ�ٶ�
	GetCharacterMovement()->AirControl = 0.2f;//���пɿس̶�


}


void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() called!"));



	UE_LOG(LogTemp, Warning, TEXT("Name of instance:%s"), *GetName());	
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{	
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction, Value);	
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	/*ͨ������һ�������ͳ��Ի���ת���ٳ���Delta time��ȷ������ת���ת��*/
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::FireWeapon()
{

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);//���ſ�ǹ����
	}
		
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");//���ǹ�ܲ���
	if (BarrelSocket)
	{
		const FTransform SocketTransform=BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash) {
		
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);//����ǹ�ڻ���
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd) 
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);//����ײ��Ч������
				
			}
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam)
			{

				Beam->SetVectorParameter(FName("Target"), BeamEnd);

			}
		}
	}

	//��ȡ����ʼ��ʵ�������ſ��𶯻���̫��
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{
	//��ȡ�ӿڵĴ�С
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//��ȡ��Ļ׼��λ��
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection; 

	//��ȡ����λ�ú�׼�Ƿ���
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld) //�����Ƿ�ͶӰ�ɹ�
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };


		// �����ӵ�����λ��
		OutBeamLocation = End;

		//����Ļ׼������Ϸ�����λ�ý�����׷��
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit)//�������������򷵻ػ��е�λ��
		{
			
			OutBeamLocation = ScreenTraceHit.Location;

		}

		//��ǹ�ܴ��������׷�٣���ֹ��Ļ�������֮�����赲,��ǹ���������֮��������
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);
		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}

		return true;
	}
	return false;
}


void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}


void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	//�����밴��
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	//ֱ�ӵ��ú���ʵ������ƶ���ͷ
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, & ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

}

