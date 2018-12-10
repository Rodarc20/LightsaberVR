// Fill out your copyright notice in the Description page of Project Settings.

#include "Escudo.h"
#include "Public/UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "ProyectilEnemigo.h"
#include "Components/CapsuleComponent.h"


AEscudo::AEscudo() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EscudoMango = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EscudoMango"));
	RootComponent = EscudoMango;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> MangoAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> MangoMaterial(TEXT("Material'/Game/LightsaberVR/Materials/HandleSaberMaterial.HandleSaberMaterial'"));


	EscudoMango->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	if (MangoAsset.Succeeded()) {
		EscudoMango->SetStaticMesh(MangoAsset.Object);
		if (MangoMaterial.Succeeded()) {
			EscudoMango->SetMaterial(0, MangoMaterial.Object);
		}
	}
	EscudoMango->SetCollisionProfileName(TEXT("Arma"));
	EscudoMango->SetSimulatePhysics(true);

	Escudo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Escudo"));
	Escudo->SetupAttachment(EscudoMango);



	static ConstructorHelpers::FObjectFinder<UStaticMesh> EscudoAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> EscudoMaterial(TEXT("Material'/Game/LightsaberVR/Materials/EscudoMaterial.EscudoMaterial'"));

	Escudo->SetWorldScale3D(FVector(1.0f, 0.1f, 1.0f));
	Escudo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Escudo->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	if (EscudoAsset.Succeeded()) {
		Escudo->SetStaticMesh(EscudoAsset.Object);
		if (EscudoMaterial.Succeeded()) {
			Escudo->SetMaterial(0, EscudoMaterial.Object);
		}
	}

	ColisionEscudo = CreateDefaultSubobject<UBoxComponent>(TEXT("ColisionEscudo"));
	ColisionEscudo->SetupAttachment(RootComponent);
	ColisionEscudo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ColisionEscudo->SetRelativeScale3D(FVector(0.0f, 0.0f, 0.0f));
	ColisionEscudo->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	ColisionEscudo->SetBoxExtent(FVector(50.0f, 700.0f, 700.0f));
	ColisionEscudo->OnComponentBeginOverlap.AddDynamic(this, &AEscudo::OnBeginOverlapEscudo);
	ColisionEscudo->OnComponentEndOverlap.AddDynamic(this, &AEscudo::OnEndOverlapEscudo);
    //ColisionEscudo->Activate(false);

	VelocidadActivacion = 20.0f;//escala 1 por segundo
	bAnimando = false;

	bActivado = false;
	Escudo->SetVisibility(false);


	//Escudo->SetRelativeScale3D(FVector(0.1f, 10.0f, 10.0f));

}


void AEscudo::BeginPlay() {
	Super::BeginPlay();

}

void AEscudo::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (bAnimando) {
		if (bActivado) {//he activado, y esto animando, osea sale la espada hasta escala 1
			FVector Escala = Escudo->GetRelativeTransform().GetScale3D();
			Escala += FVector(0.0f, VelocidadActivacion*DeltaTime, VelocidadActivacion*DeltaTime);
			if (Escala.Z >= 10.0f) {
				Escala.Z = 10.0f;
			}
			if (Escala.Y >= 10.0f) {
				Escala.Y = 10.0f;
			}
			if (Escala.Z == 10.0f && Escala.Y == 10.0f) {
				bAnimando = false;
			}
			Escudo->SetRelativeScale3D(Escala);
		}
		else {//he activado, y esto animando, osea sale la espada hasta escala 1
			FVector Escala = Escudo->GetRelativeTransform().GetScale3D();
			Escala += FVector(-0.0f, -VelocidadActivacion * DeltaTime, -VelocidadActivacion * DeltaTime);
			if (Escala.Z <= 0.0f) {
				Escala.Z = 0.0f;
			}
			if (Escala.Y <= 0.0f) {
				Escala.Y = 0.0f;
			}
			if (Escala.Z == 0.0f && Escala.Y == 0.0f) {
				bAnimando = false;
				Escudo->SetVisibility(false);
                //ColisionEscudo->Activate(false);
                ColisionEscudo->SetRelativeScale3D(FVector(0.0f, 0.0f, 0.0f));
			}
			Escudo->SetRelativeScale3D(Escala);
		}
	}
}

void AEscudo::AccionPrincipal() {
	if (bActivado) {
		bActivado = false;
		bAnimando = true;
	}
	else {
		bActivado = true;
		bAnimando = true;
		Escudo->SetVisibility(true);
        //ColisionEscudo->Activate(true);
        ColisionEscudo->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		//debo iniciar animacion
	}
}

void AEscudo::AccionSecundaria() {
}

void AEscudo::Sujetar(UMotionControllerComponent * Controller) {
    Holder = Controller;
	EscudoMango->SetSimulatePhysics(false);
	AttachToComponent(Controller, FAttachmentTransformRules::KeepRelativeTransform);
	SetActorRelativeLocation(FVector(0.0, 0.0, -2.0f));
	SetActorRelativeRotation(FRotator(270.0f, 0.0f, 0.0f));
}

void AEscudo::Soltar() {
	EscudoMango->SetSimulatePhysics(true);
    Holder = nullptr;
}


void AEscudo::OnBeginOverlapEscudo(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherActor != GetOwner())) { //no es necesario el ultimo, solo para este caso particular en el que no quiero que el propio conejo active esta funconalidad
        TocarAlgo();
		AProyectilEnemigo * const Proyectil = Cast<AProyectilEnemigo>(OtherActor);
		if (Proyectil && !Proyectil->IsPendingKill()) {
			USphereComponent * const ColisionProyectil = Cast<USphereComponent>(OtherComp);//para la casa no necesito verificar que haya tocado su staticmesh
			if (ColisionProyectil) {
				Proyectil->RecibirGolpe();
			}
		}
	}
}

void AEscudo::OnEndOverlapEscudo(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex) {
}

void AEscudo::TocarAlgo_Implementation() {
    //que vibre algo
}



