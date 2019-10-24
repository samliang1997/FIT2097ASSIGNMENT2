// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPack.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
//#include "FIT2097Assignment2Character.h"


// Sets default values
AHealthPack::AHealthPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	//SphereRadius = 100.0f;
	//MyCollisionShpere = CreateDefaultSubobject<USphereComponent>(TEXT("my sphere component"));
	//MyCollisionShpere->InitSphereRadius(SphereRadius);
	//MyCollisionShpere->SetCollisionProfileName("Trigger");
	//RootComponent = MyCollisionShpere;

	//MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("my mesh"));
	//MyMesh->SetupAttachment(RootComponent);

	//OnActorBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlap);

}

 //Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

/*
void AHealthPack::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if(OtherActor != nullptr && OtherActor != this)
	{
		
		MyCharacter = Cast<AFIT2097Assignment2Character>(OtherActor);

		if(MyCharacter&& MyCharacter->GetHealth()<100.0f || MyCharacter->GetJoy()<100.0f)
		{
			MyCharacter->UpdateHealth(20.0f);
			MyCharacter->UpdateJoy(20.0f);
			Destroy();
		}
	}
}
*/
