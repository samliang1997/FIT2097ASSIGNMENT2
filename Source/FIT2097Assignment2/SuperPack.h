// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SuperPack.generated.h"

UCLASS()
class FIT2097ASSIGNMENT2_API ASuperPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASuperPack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//UFUNCTION()
	//	void OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor);

	//UPROPERTY(EditAnywhere)
	//	AFIT2097Assignment2Character* MyCharacter;

};
