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

	/**角色移动速度*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Movement, meta = (AllowPrivateAccess = "true"))
    float Speed;


	/*判断角色是否在空气中**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/*判断角色是否正在移动**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* 瞄准位置和移动方向的偏移量，用于非正向移动腿部动作变化 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw; 

	/*保存停止移动前的偏移量，确保停止动作正常，否则停止后偏移速度向量为零，偏移量消失，无法做出对应左右后方向的停止动作*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
};
