#include "TreeNodeDynamic.h"

DEFINE_LOG_CATEGORY(LogTree);

void DumpTree(UTreeNodeDynamic* node, int d)
{
	FString space;
	for (int i = 0; i < d; i++) space += "  ";
	UE_LOG(LogTree, Warning, TEXT("%s%s (%s)"), *space, *node->name.ToString(), *node->GetName());
	for (int i = 0; i < node->children.Num(); i++) DumpTree(node->children[i], d + 1);
}

bool RecursiveNodeCheck(UTreeNodeDynamic* S, UTreeNodeDynamic* P, TMap<FName, UTreeNodeDynamic*>* customTreesBuffer, TMap<FName, FName>* customRunesBuffer, int d)
{
	// debug stuff
	FString space;
	for (int i = 0; i < d; i++) space += "  ";

	UE_LOG(LogTree, Warning, TEXT("%s%s x %s"), *space, *S->name.ToString(), *P->name.ToString());

	if (P->IsTree())
	{
		customTreesBuffer->Add(FName(*P->name.ToString().RightChop(1)), S);
		UE_LOG(LogTree, Warning, TEXT("%s%s is a tree, success"), *space, *S->name.ToString());
		return true;
	}
	else if (P->IsPlace())
	{
		customRunesBuffer->Add(FName(*(P->name.ToString().RightChop(1))), S->name);
		UE_LOG(LogTree, Warning, TEXT("%s%s is a rune"), *space, *S->name.ToString());
	}
	else if (S->name != P->name)
	{
		UE_LOG(LogTree, Warning, TEXT("%s%s is not a %s, fail"), *space, *S->name.ToString(), *P->name.ToString());
		return false;
	}
	else if (S->children.Num() < P->RuneChildren().Num())
	{
		UE_LOG(LogTree, Warning, TEXT("%s%s has %d children against %d(%d), fail"), *space, *S->name.ToString(), S->children.Num(), P->RuneChildren().Num(), P->children.Num());
		return false;
	}

	TArray<bool> tested;
	tested.Init(false, S->children.Num());

	for (int i = 0; i < S->children.Num(); i++)
	{
		bool success = false;

		//match patterns only after normal nodes
		for (int killmeplez = 0; killmeplez <= 1; killmeplez++)
		{
			for (int j = 0; j < P->children.Num(); j++)
			{
				if (tested[j]) continue;
				if ((bool)killmeplez != P->children[j]->IsPattern()) continue;

				TMap<FName, UTreeNodeDynamic*> treeBuff;
				TMap<FName, FName> runeBuff;

				if (RecursiveNodeCheck(S->children[i], P->children[j], &treeBuff, &runeBuff, d + 1))
				{
					tested[j] = true;

					TArray<FName> tnames, rnames;
					
					treeBuff.GetKeys(tnames);
					runeBuff.GetKeys(rnames);
					
					UE_LOG(LogTree, Warning, TEXT("%s%s has %d trees and %d runes"), *space, *S->name.ToString(), tnames.Num(), rnames.Num());

					for (int k = 0; k < tnames.Num(); k++)
					{
						customTreesBuffer->Add(tnames[k], treeBuff[tnames[k]]);
						UE_LOG(LogTree, Warning, TEXT("%sT: %s (%s)"), *space, *tnames[k].ToString(), *treeBuff[tnames[k]]->GetName());
					}

					for (int k = 0; k < rnames.Num(); k++)
					{
						customRunesBuffer->Add(rnames[k], runeBuff[rnames[k]]);
						UE_LOG(LogTree, Warning, TEXT("%sR: %s (%s)"), *space, *rnames[k].ToString(), *runeBuff[rnames[k]].ToString());
					}

					success = true;
				}
			}
			if (success) break;
		}

		if (!success) return false;
	}

	return true;
}

void UTreeNodeDynamic::MatchTrees(UTreeNodeDynamic* source, UTreeNodeDynamic* pattern, TMap<FName, UTreeNodeDynamic*>& matchedTrees, TMap<FName, FName>& matchedRunes, bool& success)
{
	TMap<FName, UTreeNodeDynamic*> customTreesBuffer;
	TMap<FName, FName> customRunesBuffer;

	UE_LOG(LogTree, Warning, TEXT("\nTree match operation started..."));

	// DumpTree(source, 0);
	// DumpTree(pattern, 0);

	if (RecursiveNodeCheck(source, pattern, &customTreesBuffer, &customRunesBuffer, 0))
	{
		success = true;
		matchedTrees = customTreesBuffer;
		matchedRunes = customRunesBuffer;
	}
	else
	{
		success = false;
	}
}

bool UTreeNodeDynamic::IsTree()
{
	return name.ToString().Len() > 1 && name.ToString()[0] == '#';
}

bool UTreeNodeDynamic::IsPlace()
{
	return name.ToString().Len() > 1 && name.ToString()[0] == '?';
}

bool UTreeNodeDynamic::IsPattern()
{
	return IsTree() || IsPlace();
}

TArray<UTreeNodeDynamic*> UTreeNodeDynamic::RuneChildren()
{
	TArray<UTreeNodeDynamic*> result;
	for (int i = 0; i < children.Num(); i++) if (!children[i]->IsPattern()) result.Add(children[i]);
	return result;
}

TArray<UTreeNodeDynamic*> UTreeNodeDynamic::PatternChildren()
{
	TArray<UTreeNodeDynamic*> result;
	for (int i = 0; i < children.Num(); i++) if (children[i]->IsPattern()) result.Add(children[i]);
	return result;
}
