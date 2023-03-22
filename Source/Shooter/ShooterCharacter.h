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


	/**ǰ���ƶ�����*/
	void MoveForward(float Value);

    /*�����ƶ�����*/	
	void MoveRight(float Value);

	/**�����ӽ��ƶ�*/
	void TurnAtRate(float Rate);

	/*�����ӽ��ƶ�*/
	void LookUpAtRate(float Rate);

    /*һ������*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**meta = (AllowPrivateAccess = "true")��Ϊ����˽�в��ֱ�¶����ͼ�����Ƿ���*/
	/**��������ˣ����ɱۣ����������λ�ڽ�ɫ���沢�ҷ�ֹ��ײ*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**Camera that follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**�ֱ�����ת���ٶȣ��ԡ�/��Ϊ��λ��*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category=Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**�ֱ�����ת��*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/**���ǹ����ʾ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/**ǹ����������ϵͳ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/*�������̫��**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/**�ӵ���ײ����������*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/**�ӵ�����켣*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;



public:
	/**Returns CameraBoom subobject(�Ӷ���),���Ϊ�����������Խ�ʡ�ռ�*/
	FORCEINLINE USpringArmComponent* GetCameraBoom()const { return CameraBoom ; }
	
	/**Returns FollowCamera subobject(�Ӷ���)*/
	FORCEINLINE UCameraComponent* GetFollowCamera()const { return FollowCamera ; }


};
