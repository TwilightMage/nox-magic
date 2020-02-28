#include "PersistentWorldProfile.h"
#include "NoxMagicFunctions.h"
#include "FileHelper.h"
#include "Paths.h"
#include <string>
#include <filesystem>

using namespace std;
using namespace std::experimental;

#define normalstring(str)	string(TCHAR_TO_UTF8(*str))

#define GetPath(profileName)							(FPaths::ProjectDir() + "Saves/" + profileName + "/" + profileName + ".prf")

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
	FFileHelper::SaveArrayToFile(TArrayView<uint8>(bytes), *GetPath(profileName));
}

UPersistentWorldProfile * UPersistentWorldProfile::Load(ELoadState& branch)
{
	

	if (filesystem::exists(normalstring(GetPath(profileName))))
	{
		branch = ELoadState::Success;
		TArray<uint8> bytes;
		FFileHelper::LoadFileToArray(bytes, *GetPath(profileName));
		UPersistentWorldProfile* result = UNoxMagicFunctions::DeserializeObject<UPersistentWorldProfile>(bytes);
		result->profileName = profileName;
		return result;
	}
	else
	{
		branch = ELoadState::SlotDoesntExist;
		return nullptr;
	}
}
