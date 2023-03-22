// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DealtaTime)
{
	//ȷ�����ǿ�ָ��
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	//ȷ��PawnOwner �Ƿ�ɹ�CastΪAShooterCharater
	if (ShooterCharacter)
	{
		//��ý�ɫ�ٶ�
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		//��ˮƽ�ٶ���0��ֻ��ȡ�����ٶ�
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//�жϽ�ɫ�Ƿ��ڿ���
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();


		//�Ƿ��ڼ���(ͨ�������ٶ������Ĵ�С�Ƿ�Ϊ0)
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//��ȡ�����ɫ�ĳ�����ƶ����򣬲����������������֮���ƫ��ֵ���ڽ�ɫ�ƶ�ʱʹ�ø�ƫ��ֵ�����������ɫ���ƶ��������š�
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();//��ȡ�����ɫ��ǰ��׼�ĳ���
		FRotator MovementRotation =UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());//��ȡ�����ɫ��ǰ���ٶ�����������X����ת90�ȣ��Ӷ��õ�һ��ƽ���ڵ������תֵ��
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw; 

		//��ֹͣ�ƶ�ǰ��������״ֵ̬,�Ա�ֹͣ��������ȷ����.
		if (ShooterCharacter->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
	

	}
}



//��ʼ��
void UShooterAnimInstance::NativeInitializeAnimation()
{

	//��ΪTryGetPawnOwner()ֻ�ܷ���һ��pawn������Ҫǿ��ת����Character
	 ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

}



