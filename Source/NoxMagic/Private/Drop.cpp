#include "Drop.h"
#include "Interactive.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "ADrop"

ADrop::ADrop()
{
	PrimaryActorTick.bCanEverTick = true;

	billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));

	interactive = CreateDefaultSubobject<UInteractive>(TEXT("Interactive"));
	interactive->SetAction(LOCTEXT("Default interaction action", "Pick"));

	FScriptDelegate interactDelegate;
	interactDelegate.BindUFunction(this, "OnInteract");
	interactive->onInteract.Add(interactDelegate);

	FScriptDelegate hightlightDelegate;
	hightlightDelegate.BindUFunction(this, "OnHightLight");
	interactive->onHighlight.Add(hightlightDelegate);

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletalmesh"));
}

#if WITH_EDITOR
void ADrop::OnConstruction(const FTransform & Transform)
{
	if (item.data.id.IsNone())
	{
		item.data.id == "Undefined";
	}

	// TODO: fetch mesh

	billboard->SetVisibility(item.data.mesh == nullptr);
}
#endif

void ADrop::LoadContentFromDatabase()
{
	switch (item.data.itemType)
	{
	case EItemType::MeleeWeapon:

		break;
	}
}

void ADrop::Disapear_Implementation(FLinearColor color)
{
	FOnTimelineFloat onProcessCallback;
	FOnTimelineEventStatic onFinishedCallback;

	FVector meshOrigin;
	FVector meshBoxExtent;
	float meshSphereRadius;
	UKismetSystemLibrary::GetComponentBounds(mesh, meshOrigin, meshBoxExtent, meshSphereRadius);

	UParticleSystemComponent* ps = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),  ConstructorHelpers::FObjectFinder<UParticleSystem>(TEXT("/Game/ParticleSystems/DropDisapear")).Object, FTransform::Identity);
	ps->SetActorParameter("Actor", this);
	//ps->SetColorParameter("Color", color);
	ps->SetFloatParameter("Density", meshSphereRadius);

	mat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(ps, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("/Game/Materials/Disolve/DisolveInstance")).Object);
	mat->SetVectorParameterValue("Color", color);
	mesh->SetSimulatePhysics(false);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mesh->SetMaterial(0, Cast<UMaterialInterface>(mat));

	onProcessCallback.BindUFunction(this, "DisolveProcess");
	onFinishedCallback.BindUFunction(this, "DisolveFinished");

	UCurveFloat* curve = NewObject<UCurveFloat>();
	//curve->GetCurves()[0].CurveToEdit->

	DisolveTimeLine = NewObject<UTimelineComponent>(this, FName("DisolveTimeLine"));
	DisolveTimeLine->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	this->BlueprintCreatedComponents.Add(DisolveTimeLine);
	DisolveTimeLine->SetNetAddressable();
	DisolveTimeLine->SetPropertySetObject(this);
	DisolveTimeLine->SetDirectionPropertyName("Direction");
	DisolveTimeLine->SetLooping(false);
	DisolveTimeLine->SetTimelineLength(1.f);
	DisolveTimeLine->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	DisolveTimeLine->SetPlaybackPosition(0.f, false);
	DisolveTimeLine->AddInterpFloat(curve, onProcessCallback);
	DisolveTimeLine->SetTimelineFinishedFunc(onFinishedCallback);

	SetLifeSpan(5.f);
}

void ADrop::DisolveProcess(float progress)
{
	mat->SetScalarParameterValue("Time", progress);
}

void ADrop::DisolveFinished()
{
	mesh->DestroyComponent();
}

void ADrop::OnInteract(APlayCharacter* interactor)
{
	// TODO: Add item to inventory
}

void ADrop::OnHightLight(bool state)
{
	mesh->SetRenderCustomDepth(state);
}

void ADrop::BeginPlay()
{
	Super::BeginPlay();

	if (!item.data.id.IsNone())
	{
		LoadContentFromDatabase();
	}

	interactive->SetName(item.data.name);
}
