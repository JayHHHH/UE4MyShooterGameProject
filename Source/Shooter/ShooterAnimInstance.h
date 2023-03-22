// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DealtaTime);

	virtual void NativeInitializeAnimation() override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	/**��ɫ�ƶ��ٶ�*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Movement, meta = (AllowPrivateAccess = "true"))
    float Speed;


	/*�жϽ�ɫ�Ƿ��ڿ�����**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/*�жϽ�ɫ�Ƿ������ƶ�**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* ��׼λ�ú��ƶ������ƫ���������ڷ������ƶ��Ȳ������仯 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw; 

	/*����ֹͣ�ƶ�ǰ��ƫ������ȷ��ֹͣ��������������ֹͣ��ƫ���ٶ�����Ϊ�㣬ƫ������ʧ���޷�������Ӧ���Һ����ֹͣ����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
};
