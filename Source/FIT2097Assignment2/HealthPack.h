// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "FIT2097Assignment2Character.h"
#include "HealthPack.generated.h"


UCLASS()
class FIT2097ASSIGNMENT2_API AHealthPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(VisibleAnywhere)
	//	class USphereComponent* MyCollisionShpere;

	//UPROPERTY(VisibleAnywhere)
	//class UStaticMeshComponent* MyMesh;
	 
	//float SphereRadius;

	//UFUNCTION()
		//void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION()
	//void OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor);

	//UPROPERTY(EditAnywhere)
	//AFIT2097Assignment2Character* MyCharacter;

};
