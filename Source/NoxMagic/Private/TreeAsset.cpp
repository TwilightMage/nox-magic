#include "TreeAsset.h"

#define min(a, b)		(a<b?a:b)

uint8 fstrcomp(FString a, FString b)
{
	for (int i = 0; i < min(a.Len(), b.Len()); i++)
	{
		if (a[i] < b[i]) return 0;
		else if (a[i] > b[i]) return 2;
	}

	if (a.Len() < b.Len()) return 0;
	if (a.Len() > b.Len()) return 2;
	return 1;
}

UTreeNodeDynamic* UTreeAsset::FormTree(TArray<FTreeNode> inputNodes, TMap<int, int> inputParents)
{
	if (inputNodes.Num() == 0) return nullptr;

	TArray<UTreeNodeDynamic*> dynamicNodes;

	for (int i = 0; i < inputNodes.Num(); i++)
	{
		UTreeNodeDynamic* node = NewObject<UTreeNodeDynamic>(GetTransientPackage());
		node->name = inputNodes[i].name;
		dynamicNodes.Add(node);
	}

	TArray<int> parentsKeys;
	inputParents.GetKeys(parentsKeys);

	for (int i = 0; i < parentsKeys.Num(); i++)
	{
		int child = parentsKeys[i];
		int parent = inputParents[parentsKeys[i]];
		dynamicNodes[child]->parent = dynamicNodes[parent];
		dynamicNodes[parent]->children.Add(dynamicNodes[child]);
	}

	for (int i = 0; i < dynamicNodes.Num(); i++) if (dynamicNodes[i]->parent == nullptr) return dynamicNodes[i];
	return nullptr;
}

UTreeNodeDynamic* UTreeAsset::FormTreeFromAsset(UTreeAsset* asset)
{
	return FormTree(asset->nodes, asset->parents);
}
