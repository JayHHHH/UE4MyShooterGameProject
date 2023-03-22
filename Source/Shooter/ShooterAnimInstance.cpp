// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DealtaTime)
{
	//确保不是空指针
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	//确认PawnOwner 是否成功Cast为AShooterCharater
	if (ShooterCharacter)
	{
		//获得角色速度
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		//把水平速度置0，只获取横向速度
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//判断角色是否在空中
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();


		//是否在加速(通过看加速度向量的大小是否为0)
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//获取射击角色的朝向和移动方向，并计算出这两个方向之间的偏差值，在角色移动时使用该偏差值来调整射击角色的移动动画播放。
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();//获取射击角色当前瞄准的朝向
		FRotator MovementRotation =UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());//获取射击角色当前的速度向量将其沿X轴旋转90度，从而得到一个平行于地面的旋转值。
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw; 

		//在停止移动前保留最后的状态值,以便停止动画的正确播放.
		if (ShooterCharacter->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
	

	}
}



//初始化
void UShooterAnimInstance::NativeInitializeAnimation()
{

	//因为TryGetPawnOwner()只能返回一个pawn所以需要强制转换成Character
	 ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

}



