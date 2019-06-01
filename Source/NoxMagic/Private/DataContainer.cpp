#include "DataContainer.h"

UDataContainer* UDataContainer::container = nullptr;

UDataContainer* UDataContainer::InitializeContainer(TSubclassOf<UDataContainer> containerType)
{
	if (*containerType && (*containerType != UDataContainer::StaticClass()))
	{
		container = NewObject<UDataContainer>(GetTransientPackage(), containerType, "data container", EObjectFlags::RF_MarkAsRootSet);
		return container;
	}
	return nullptr;
}

UDataContainer* UDataContainer::GetContainer(TSubclassOf<UDataContainer> containerType, EGetContainerBranch& branch)
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
