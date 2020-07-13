#include "NMDataContainer.h"

UNMDataContainer* UNMDataContainer::container = nullptr;

UNMDataContainer* UNMDataContainer::InitializeContainer(TSubclassOf<UNMDataContainer> containerType)
{
	if (*containerType && (*containerType != UNMDataContainer::StaticClass()))
	{
		container = NewObject<UNMDataContainer>(GetTransientPackage(), containerType, "data container", EObjectFlags::RF_MarkAsRootSet);
		return container;
	}
	return nullptr;
}

UNMDataContainer* UNMDataContainer::GetContainer(TSubclassOf<UNMDataContainer> containerType, EGetContainerBranch& branch)
{
	if (container != nullptr && container->IsValidLowLevel() && container->IsA(containerType))
	{
		branch = EGetContainerBranch::Success;
		return container;
	}
	else
	{
		branch = EGetContainerBranch::Fail;
		return nullptr;
	}
}

bool UNMDataContainer::DoesContainerExists(TSubclassOf<UNMDataContainer> containerType)
{
	return container != nullptr && container->IsValidLowLevel() && container->IsA(containerType);
}
