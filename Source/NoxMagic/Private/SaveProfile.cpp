#include "SaveProfile.h"
#include "NoxMagicFunctions.h"
#include <string>
#include "Paths.h"
#include <filesystem>
#include <chrono>
#include "FileHelper.h"
#include "SaveSlotMeta.h"

using namespace std;
using namespace std::experimental;
using namespace std::chrono_literals;

#define normalstring(str)						(string(TCHAR_TO_UTF8(*str)))

#define GetSavesDir()							(FPaths::ProjectDir() + "Saves/")
#define GetProfilePath(profileName)				(GetSavesDir() + profileName + "/")
#define GetSlotPath(profileName, slotName)		(GetProfilePath(profileName) +  slotName + ".sav")
#define GetMetaPath(profileName, slotName)		(GetProfilePath(profileName) +  slotName + ".meta")

FString USaveProfile::GetName()
{
	return profileName;
}

USaveProfile* USaveProfile::GetSaveProfile(TSubclassOf<USaveProfile> profileClass, FString profileName)
{
	if (*profileClass && (*profileClass != USaveProfile::StaticClass()))
	{
		USaveProfile* result = NewObject<USaveProfile>(GetTransientPackage(), profileClass);
		result->profileName = profileName;
		return result;
	}
	else
	{
		return nullptr;
	}
}

void USaveProfile::QuickSave(int savegameBufferSize, UTexture2D* thumb, FSaveMeta& meta)
{
	for (int i = savegameBufferSize - 1; i >= 0; i--)
	{
		if (DoesSlotExists("QuickSave " + i))
		{
			if (DoesSlotExists("QuickSave " + (i + 1))) filesystem::remove(*GetSlotPath(profileName, "QuickSave " + FString::FromInt(i + 1)));
			filesystem::rename(*GetSlotPath(profileName, "QuickSave " + FString::FromInt(i)), *GetSlotPath(profileName, "QuickSave " + FString::FromInt(i + 1)));
		}
	}
	Save("QuickSave 0", thumb, meta);
}

USaveProfile* USaveProfile::QuickLoad(ELoadState& branch)
{
	TArray<FSaveMeta> slots = GetSlots();
	if (slots.Num() == 0)
	{
		branch = ELoadState::SlotDoesntExist;
		return nullptr;
	}
	else
	{
		return Load(slots.Last().name, branch);
	}
}

void USaveProfile::Save(FString slotName, UTexture2D* thumb, FSaveMeta& meta)
{
	TArray<uint8> bytes = UNoxMagicFunctions::SerializeObject(this);
	FFileHelper::SaveArrayToFile(TArrayView<uint8>(bytes), *GetSlotPath(profileName, slotName));
	meta = USaveSlotMeta::Save(GetMetaPath(profileName, slotName), FDateTime::Now(), thumb)->GetStruct();
}

USaveProfile* USaveProfile::Load(FString slotName, ELoadState& branch)
{
	if (DoesSlotExists(slotName))
	{
		branch = ELoadState::Success;
		USaveProfile* result = NewObject<USaveProfile>(GetTransientPackage(), GetClass());
		TArray<uint8> bytes;
		FFileHelper::LoadFileToArray(bytes, *GetSlotPath(profileName, slotName));
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

bool USaveProfile::DoesProfileExists(FString profileName)
{
	return filesystem::exists(*GetProfilePath(profileName)) && filesystem::exists(*(GetProfilePath(profileName) + "/" + profileName + ".prf"));
}

bool USaveProfile::DoesSlotExists(FString slotName)
{
	return filesystem::exists(*GetSlotPath(profileName, slotName)) && filesystem::exists(*GetMetaPath(profileName, slotName));
}

TArray<FSaveMeta> USaveProfile::GetSlots()
{
	TArray<FSaveMeta> result;
	FString path = GetProfilePath(profileName);
	if (filesystem::exists(*path))
	{
		for (const auto& entry : filesystem::directory_iterator(*path))
		{
			filesystem::path p = entry.path();
			if (filesystem::is_regular_file(p) && p.extension() == ".meta")
			{
				try
				{
					USaveSlotMeta* meta = USaveSlotMeta::Load(FString(p.c_str()));
					result.Add(meta->GetStruct());
				}
				catch (...)
				{

				}				
			}
		}
	}

	for (int i = 0; i < result.Num(); i++)
	{
		for (int j = i; j > 0; j--)
		{
			if (result[j].time < result[j - 1].time)
			{
				result.Swap(j, j - 1);
			}
		}
	}

	return result;
}

TArray<FString> USaveProfile::GetProfiles()
{
	TArray<FString> result;

	for (const auto& entry : filesystem::directory_iterator(*GetSavesDir()))
	{
		filesystem::path p = entry.path();
		if (filesystem::is_directory(p) && filesystem::exists(*(FString(p.c_str()) + "/" + FString(p.stem().c_str()) + ".prf")))
		{
			result.Add(FString(p.stem().c_str()));
		}
	}

	return result;
}

void USaveProfile::ClearProfile()
{
	filesystem::remove_all(*GetProfilePath(profileName));
}
