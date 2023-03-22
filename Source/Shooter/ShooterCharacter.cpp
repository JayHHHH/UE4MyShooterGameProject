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


	//创造一个相机摇杆
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);//将摇杆依附在根组件上
	CameraBoom->TargetArmLength = 300.f;//相机跟随在人物后面的距离
	CameraBoom->bUsePawnControlRotation = true; //玩家控制人物移动旋转相机
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	//创造第三人称的相机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//将相机连到摇杆末端
	FollowCamera->bUsePawnControlRotation = false;//第三人称相机不需要跟随人物旋转

	//不要让人物跟随控制器旋转,只旋转相机就行
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement=false;//键盘偏移不再控制人物方向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//旋转速度
	GetCharacterMovement()->JumpZVelocity = 600.f;//跳跃速度
	GetCharacterMovement()->AirControl = 0.2f;//空中可控程度


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
	/*通过传入一个浮点型乘以基本转速再乘以Delta time来确定左右转向的转速*/
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
		UGameplayStatics::PlaySound2D(this, FireSound);//播放开枪声音
	}
		
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");//获得枪管插座
	if (BarrelSocket)
	{
		const FTransform SocketTransform=BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash) {
		
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);//产生枪口火焰
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
					BeamEnd);//产生撞击效果粒子
				
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

	//获取并初始化实例，播放开火动画蒙太奇
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
	//获取视口的大小
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//获取屏幕准星位置
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection; 

	//获取世界位置和准星方向
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld) //检验是否投影成功
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };


		// 设置子弹命中位置
		OutBeamLocation = End;

		//从屏幕准星在游戏世界的位置进行线追踪
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit)//射线命中物体则返回击中点位置
		{
			
			OutBeamLocation = ScreenTraceHit.Location;

		}

		//从枪管处发起二次追踪，防止屏幕与击中物之间无阻挡,但枪管与击中物之间有物体
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

	//绑定输入按键
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	//直接调用函数实现鼠标移动镜头
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, & ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

}

