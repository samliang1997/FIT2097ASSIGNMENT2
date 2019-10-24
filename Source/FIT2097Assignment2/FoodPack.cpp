// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodPack.h"

// Sets default values
AFoodPack::AFoodPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//OnActorBeginOverlap.AddDynamic(this, &AFoodPack::OnOverlap);
}

// Called when the game starts or when spawned
void AFoodPack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFoodPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void AFoodPack::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor != this)
	{

		MyCharacter = Cast<AFIT2097Assignment2Character>(OtherActor);

		if (MyCharacter&& MyCharacter->GetStamina() < 100.0f || MyCharacter->GetJoy() < 100.0f)
		{
			MyCharacter->UpdateStamina(20.0f);
			MyCharacter->UpdateJoy(20.0f);
			Destroy();
		}
	}
}
*/
