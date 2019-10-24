// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperPack.h"

// Sets default values
ASuperPack::ASuperPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//OnActorBeginOverlap.AddDynamic(this, &ASuperPack::OnOverlap);
}

// Called when the game starts or when spawned
void ASuperPack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASuperPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void ASuperPack::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor != this)
	{

		MyCharacter = Cast<AFIT2097Assignment2Character>(OtherActor);

		if (MyCharacter&& MyCharacter->GetJoy() < 100.0f|| MyCharacter->GetHealth()<100.0f|| MyCharacter->GetStamina()<100.0f)
		{
			MyCharacter->UpdateHealth(50.0f);
			MyCharacter->UpdateStamina(50.0f);
			MyCharacter->UpdateJoy(50.0f);
			Destroy();
		}
	}
}
*/
