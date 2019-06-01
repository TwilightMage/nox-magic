#include "PersistentWorldProfile.h"
#include "NoxMagicFunctions.h"
#include "FileHelper.h"
#include "Paths.h"
#include <string>
#include <filesystem>

using namespace std;
using namespace std::experimental;

#define normalstring(str)	string(TCHAR_TO_UTF8(*str))

string GetPath(FString profileName)
{
#if WITH_EDITOR
	return normalstring(FPaths::ProjectDir()) + "Saves/" + normalstring(profileName) + "/" + normalstring(profileName) + ".prf";
#else
	return normalstring(FPaths::GametDir()) + "Saves/" + normalstring(profileName) + "/" + normalstring(profileName) + ".prf";
#endif
}

FString UPersistentWorldProfile::GetName()
{
	return profileName;
}

UPersistentWorldProfile * UPersistentWorldProfile::GetWorldProfile(TSubclassOf<UPersistentWorldProfile> profileClass, FString profileName)
{
	if (*profileClass && (*profileClass != UPersistentWorldProfile::StaticClass()))
	{
		UPersistentWorldProfile* result = NewObject<UPersistentWorldProfile>(GetTransientPackage(), profileClass);
		result->profileName = profileName;
		return result;
	}
	else
	{
		return nullptr;
	}
}

void UPersistentWorldProfile::Save()
{
	TArray<uint8> bytes = UNoxMagicFunctions::SerializeObject(this);
	string path = GetPath(profileName);
	FFileHelper::SaveArrayToFile(TArrayView<uint8>(bytes), wstring(path.begin(), path.end()).c_str());
}

UPersistentWorldProfile * UPersistentWorldProfile::Load(ELoadState& branch)
{
	

	if (filesystem::exists(GetPath(profileName)))
	{
		branch = ELoadState::Success;
		UPersistentWorldProfile* result = NewObject<UPersistentWorldProfile>(GetTransientPackage(), GetClass());
		TArray<uint8> bytes;
		string path = GetPath(profileName);
		FFileHelper::LoadFileToArray(bytes, wstring(path.begin(), path.end()).c_str());
		UNoxMagicFunctions::DeserializeObject(result, bytes);
		result->profileName = profileName;
		return result;
	}
	else
	{
		branch = ELoadState::SlotDoesntExist;
		return nullptr;
	}
}
