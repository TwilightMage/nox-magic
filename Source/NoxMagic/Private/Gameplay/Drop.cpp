#include "Drop.h"
#include "Kismet/GameplayStatics.h"
#include "Items/InventoryItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "NMGameMode.h"
#include "Interactive.h"
#include "NMCharacter.h"
#include "Inventory.h"
#include "Engine/Classes/GameFramework/GameMode.h"
#include "Engine/StaticMesh.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

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
	Interactive->OnHighlightStateChanged.AddDynamic(this, &ADrop::InteractiveHighlightStateChanged);
	Interactive->OnInteract.AddDynamic(this, &ADrop::InteractiveInteract);

	DisolveDuration = 1.f;
	DisolveParticlesLifetime = 2.f;
}

void ADrop::Disapear_Implementation(FLinearColor color)
{
	FVector meshOrigin;
	FVector meshBoxExtent;
	float meshSphereRadius;
	UKismetSystemLibrary::GetComponentBounds(Mesh, meshOrigin, meshBoxExtent, meshSphereRadius);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (UParticleSystemComponent* PS = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DisapearParticleSystem, FTransform::Identity))
	{
		PS->SetActorParameter("Actor", this);
		PS->SetColorParameter("Color", color);
		PS->SetFloatParameter("Density", meshSphereRadius);

		if (auto Material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(PS, DisapearMaterial))
		{
			Mat = Material;
			Mat->SetVectorParameterValue("Color", color);

			Mesh->SetMaterial(0, Cast<UMaterialInterface>(Mat));
		}
	}

	GetWorld()->GetTimerManager().SetTimer(DisolveTimerHandle, this, &ADrop::DisolveFinish, DisolveDuration, false);

	SetLifeSpan(DisolveParticlesLifetime);
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
	drop->Item->OnCountChanged.AddDynamic(drop, &ADrop::ItemCountChanged);
	drop->Item->OnSplited.AddDynamic(drop, &ADrop::ItemSplited);
	drop->Item->OnAbsorbed.AddDynamic(drop, &ADrop::ItemAbsorbed);
	drop->Item->OnOwnerChanged.AddDynamic(drop, &ADrop::ItemOwnerChanged);
	drop->Refreshed();

	return drop;
}

void ADrop::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (defaultRawID != NAME_None)
	{
		if (Item ? defaultRawID != Item->GetRawID() : defaultRawID != NAME_None)
		{
			if (UInventoryItem* newItem = UInventoryItem::CreateItem(defaultRawID))
			{
				Item = newItem;
				Item->SetNewOwner(this);
				Item->OnCountChanged.AddDynamic(this, &ADrop::ItemCountChanged);
				Item->OnSplited.AddDynamic(this, &ADrop::ItemSplited);
				Item->OnAbsorbed.AddDynamic(this, &ADrop::ItemAbsorbed);
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

	if (DisolveTimerHandle.IsValid())
	{
		Mat->SetScalarParameterValue("Time", GetWorld()->GetTimerManager().GetTimerElapsed(DisolveTimerHandle) / DisolveDuration);
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

	Interactive->SetTitle(Item ? Item->GetDefaults().Name : FText::FromString("Undefined"));
}

void ADrop::InteractiveInteract(ANMCharacter* Interactor)
{
	InteractiveInteract_SERVER(Interactor);
}

void ADrop::InteractiveInteract_SERVER_Implementation(ANMCharacter* Interactor)
{
	Interactor->PickUp(this);
}

void ADrop::InteractiveHighlightStateChanged(bool newState)
{
	Mesh->SetRenderCustomDepth(newState);
}

void ADrop::DisolveFinish()
{
	Mesh->DestroyComponent();

	DisolveTimerHandle.Invalidate();
}

void ADrop::ItemCountChanged(UInventoryItem* Sender)
{
	if (Sender == Item && !UInventoryItem::IsItemValid(Sender))
	{
		Item->SetNewOwner(GetTransientPackage());
	}
}

void ADrop::ItemSplited(UInventoryItem* Sender, UInventoryItem* AnotherPart)
{
	if (Sender == Item && !UInventoryItem::IsItemValid(Sender))
	{
		Item->SetNewOwner(GetTransientPackage());
	}
}

void ADrop::ItemAbsorbed(UInventoryItem* Sender, UInventoryItem* Into)
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
		Item->OnCountChanged.RemoveAll(this);
		Item->OnSplited.RemoveAll(this);
		Item->OnAbsorbed.RemoveAll(this);
		Item->OnOwnerChanged.RemoveAll(this);
		Item = nullptr;

		if (UKismetSystemLibrary::IsServer(this))
		{
			FLinearColor Color;
			Color.R = 1.f;
			Color.G = 0.f;
			Color.B = 1.f;
			Color.A = 1.f;
			Disapear(Color);
		}
	}
}