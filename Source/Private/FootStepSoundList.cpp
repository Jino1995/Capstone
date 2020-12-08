// Fill out your copyright notice in the Description page of Project Settings.


#include "FootStepSoundList.h"
#include "Sound/SoundBase.h"

USoundBase* UFootStepSoundList::GetSound(FString SoundName)
{
	for (auto it : SoundList)
	{
		if (it.Key == SoundName)
		{
			return it.Value;
		}
	}
	return nullptr;
}
