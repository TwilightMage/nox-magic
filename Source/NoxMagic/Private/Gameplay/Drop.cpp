#include "Drop.h"
#include "Kismet/GameplayStatics.h"
#include "Items/InventoryItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/TimelineComponent.h"
#include "NMGameMode.h"
#include "Interactive.h"
#include "NMCharacter.h"
#include "Inventory.h"
#include "Engine/Classes/GameFramework/GameMode.h"
#include "Engine/StaticMesh.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

#define LOCTEXT_NAMESPACE "Drop"

ADrop::ADrop()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootCompontnt"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	Interactive = CreateDefaultSubobject<UInteractive>(TEXT("Interactive"));
	Interactive->SetAction(LOCTEXT("PickItem", "Pick"));
	Interactive->OnHighlightStateChanged.AddDynamic(this, &ADrop::OnHighlightStateChanged);
	Interactive->OnInteract.AddDynamic(this, &ADrop::OnInteract);
}

void ADrop::Disapear_Implementation(FLinearColor color)
{
	FOnTimelineFloat onProcessCallback;
	FOnTimelineEventStatic onFinishedCallback;

	FVector meshOrigin;
	FVector meshBoxExtent;
	float meshSphereRadius;
	UKismetSystemLibrary::GetComponentBounds(Mesh, meshOrigin, meshBoxExtent, meshSphereRadius);

	UParticleSystemComponent* ps = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DisapearParticleSystem, FTransform::Identity);
	ps->SetActorParameter("Actor", this);
	//ps->SetColorParameter("Color", color);
	ps->SetFloatParameter("Density", meshSphereRadius);

	Mat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(ps, DisapearMaterial);
	Mat->SetVectorParameterValue("Color", color);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetMaterial(0, Cast<UMaterialInterface>(Mat));

	onProcessCallback.BindUFunction(this, "DisolveUpdate");
	onFinishedCallback.BindUFunction(this, "DisolveEnd");

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

ADrop* ADrop::SpawnDropRaw(UObject* WorldContextObject, FTransform Transform, FName RawID, int Count)
{
	if (UInventoryItem* item = UInventoryItem::CreateItem(RawID, Count))
	{
		return ADrop::SpawnDropPredefined(WorldContextObject, Transform, item);
	}

	return nullptr;
}

ADrop* ADrop::SpawnDropPredefined(UObject* WorldContextObject, FTransform Transform, UInventoryItem* ContainedItem)
{
	ADrop* drop = WorldContextObject->GetWorld()->SpawnActor<ADrop>(ADrop::StaticClass(), Transform);
	drop->Item = ContainedItem;
	drop->Item->SetNewOwner(drop);
	drop->Item->OnUpdated.AddDynamic(drop, &ADrop::ItemUpdated);
	drop->Item->OnSplited.AddDynamic(drop, &ADrop::ItemSplited);
	drop->Item->OnMerged.AddDynamic(drop, &ADrop::ItemMerged);
	drop->Item->OnOwnerChanged.AddDynamic(drop, &ADrop::ItemOwnerChanged);
	drop->Refreshed();

	return drop;
}

void ADrop::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (defaultRawID != NAME_None)
	{
		if (defaultRawID != CachedDefaultRawID)
		{
			CachedDefaultRawID = defaultRawID;

			if (UInventoryItem* newItem = UInventoryItem::CreateItem(defaultRawID))
			{
				Item = newItem;
				Item->SetNewOwner(this);
				Item->OnUpdated.AddDynamic(this, &ADrop::ItemUpdated);
				Item->OnSplited.AddDynamic(this, &ADrop::ItemSplited);
				Item->OnMerged.AddDynamic(this, &ADrop::ItemMerged);
				Item->OnOwnerChanged.AddDynamic(this, &ADrop::ItemOwnerChanged);
			}
		}
		else if (Item && Item->IsValidLowLevel())
		{
			if (auto newDefaults = ANMGameMode::FetchItemDefaults(defaultRawID))
			{
				Item->SetDefaults(*newDefaults);
			}
			else
			{
				Item = nullptr;
			}
		}
		Refreshed();
		return;
	}

	Item = nullptr;
	Refreshed();
}

void ADrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Item)
	{
		Item->Tick(DeltaTime);
	}
}

void ADrop::BeginPlay()
{
	Super::BeginPlay();
}

void ADrop::Refreshed()
{
	USkeletalMesh* MeshAsset = Item ? Item->GetDefaults().Mesh : nullptr;

	if (MeshAsset)
	{
		Mesh->SetSkeletalMesh(MeshAsset);
		Mesh->SetCollisionProfileName("Drop");
		Mesh->SetSimulatePhysics(true);
	}
	else
	{
		Mesh->SetSkeletalMesh(nullptr);
		Mesh->SetSimulatePhysics(false);
	}

	Billboard->SetVisibility(MeshAsset == nullptr);

	Interactive->SetName(Item ? Item->GetDefaults().Name : FText::FromString("Undefined"));
}

void ADrop::OnInteract(ANMCharacter* interactor)
{
	interactor->PickUp(this);
}

void ADrop::OnHighlightStateChanged(bool newState)
{
	Mesh->SetRenderCustomDepth(newState);
}

void ADrop::ItemUpdated(UInventoryItem * Sender)
{
	if (Sender == Item && !UInventoryItem::IsItemValid(Sender))
	{
		Item->SetNewOwner(GetTransientPackage());
	}
}

void ADrop::ItemSplited(UInventoryItem * Sender, UInventoryItem * AnotherPart)
{
	if (Sender == Item && !UInventoryItem::IsItemValid(Sender))
	{
		Item->SetNewOwner(GetTransientPackage());
	}
}

void ADrop::ItemMerged(UInventoryItem * Sender, UInventoryItem * Into)
{
	if (Sender == Item)
	{
		Item->SetNewOwner(GetTransientPackage());
	}
}

void ADrop::ItemOwnerChanged(UInventoryItem* Sender)
{
	if (Sender == Item)
	{
		Item->OnUpdated.RemoveAll(this);
		Item->OnSplited.RemoveAll(this);
		Item->OnMerged.RemoveAll(this);
		Item->OnOwnerChanged.RemoveAll(this);
		Item = nullptr;
		Destroy();
	}
}