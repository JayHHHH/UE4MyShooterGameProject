// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{

	GENERATED_BODY()



public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/**前后移动输入*/
	void MoveForward(float Value);

    /*左右移动输入*/	
	void MoveRight(float Value);

	/**左右视角移动*/
	void TurnAtRate(float Rate);

	/*上下视角移动*/
	void LookUpAtRate(float Rate);

    /*一键开火*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**meta = (AllowPrivateAccess = "true")是为了让私有部分暴露给蓝图供我们访问*/
	/**摄像机吊杆（弹簧臂）将摄像机定位在角色后面并且防止碰撞*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**Camera that follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**手柄左右转向速度，以°/秒为单位。*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**手柄上下转速*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/**随机枪声提示*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/**枪口闪光粒子系统*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/*开火的蒙太奇**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/**子弹碰撞产生的粒子*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/**子弹烟雾轨迹*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;



public:
	/**Returns CameraBoom subobject(子对象),标记为内联函数可以节省空间*/
	FORCEINLINE USpringArmComponent* GetCameraBoom()const { return CameraBoom ; }
	
	/**Returns FollowCamera subobject(子对象)*/
	FORCEINLINE UCameraComponent* GetFollowCamera()const { return FollowCamera ; }


};
