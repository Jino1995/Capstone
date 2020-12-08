// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolTipWidget.h"
#include "TextBlock.h"
#include "Image.h"
#include "Text.h"

void UToolTipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TopBox = Cast<UImage>(GetWidgetFromName(FName("TopImage")));
	Name_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Name")));
	ItemType_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Type")));
	Damage_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Damage")));
	Armor_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Armor")));
	HP_Text = Cast<UTextBlock>(GetWidgetFromName(FName("HP")));
	SP_Text = Cast<UTextBlock>(GetWidgetFromName(FName("SP")));
	ATK_Text = Cast<UTextBlock>(GetWidgetFromName(FName("ATK")));
	DEF_Text = Cast<UTextBlock>(GetWidgetFromName(FName("DEF")));
	UseInfo_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Use")));
	Description_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Description")));
	StackSize_Text = Cast<UTextBlock>(GetWidgetFromName(FName("StackSize")));
	Quality_Text = Cast<UTextBlock>(GetWidgetFromName(FName("Quality")));
	Price_Text = Cast<UTextBlock>(GetWidgetFromName(FName("PriceText")));
}

void UToolTipWidget::SetItemToolTip(FItemData& ItemData, bool bTraderSlot)
{
	switch (ItemData.GetItemType())
	{
	case EItemType::Consumable:
		Name_Text->SetText(FText::FromName(ItemData.GetName()));
		ItemType_Text->SetText(FText::FromString("Consumable"));
		UseInfo_Text->SetText(FText::FromString(ItemData.GetUse()));
		Description_Text->SetText(FText::FromString(ItemData.GetDescription()));
		StackSize_Text->SetText(FText::FromString("Max Stack : " + FString::FromInt(ItemData.GetMaxStackSize())));

		Damage_Text->RemoveFromParent();
		Armor_Text->RemoveFromParent();
		HP_Text->RemoveFromParent();
		SP_Text->RemoveFromParent();
		ATK_Text->RemoveFromParent();
		DEF_Text->RemoveFromParent();
		Quality_Text->RemoveFromParent();
		break;
	case EItemType::Equipment:
		if (ItemData.GetEquipmentType() == EEquipmentType::Armor || ItemData.GetEquipmentType() == EEquipmentType::Shield)
		{
			Name_Text->SetText(FText::FromName(ItemData.GetName()));
			ItemType_Text->SetText(FText::FromString("Equipment"));
			Armor_Text->SetText(FText::FromString(FString(FString::FromInt(ItemData.GetArmor()) + " Armor")));
			Description_Text->SetText(FText::FromString(ItemData.GetDescription()));

			if (ItemData.GetHP() > 0)
				HP_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetHP()) + " HP")));
			else
				HP_Text->RemoveFromParent();

			if(ItemData.GetSP() > 0)
				SP_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetSP()) + " SP")));
			else
				SP_Text->RemoveFromParent();

			if(ItemData.GetAtk() > 0)
				ATK_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetAtk()) + " ATK")));
			else
				ATK_Text->RemoveFromParent();

			if (ItemData.GetDef() > 0)
				DEF_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetDef()) + " DEF")));
			else
				DEF_Text->RemoveFromParent();

			UseInfo_Text->RemoveFromParent();
			Damage_Text->RemoveFromParent();
			StackSize_Text->RemoveFromParent();
		}
		else if (ItemData.GetEquipmentType() == EEquipmentType::Weapon)
		{
			Name_Text->SetText(FText::FromName(ItemData.GetName()));
			ItemType_Text->SetText(FText::FromString(ItemData.GetWeaponDetail()));
			Damage_Text->SetText(FText::FromString(FString(FString::FromInt(ItemData.GetDamage()) + " Damage")));
			Description_Text->SetText(FText::FromString(ItemData.GetDescription()));

			if (ItemData.GetHP() > 0)
				HP_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetHP()) + " HP")));
			else
				HP_Text->RemoveFromParent();

			if (ItemData.GetSP() > 0)
				SP_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetSP()) + " SP")));
			else
				SP_Text->RemoveFromParent();

			if (ItemData.GetAtk() > 0)
				ATK_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetAtk()) + " ATK")));
			else
				ATK_Text->RemoveFromParent();

			if (ItemData.GetDef() > 0)
				DEF_Text->SetText(FText::FromString(FString("+" + FString::FromInt(ItemData.GetDef()) + " DEF")));
			else
				DEF_Text->RemoveFromParent();

			Armor_Text->RemoveFromParent();
			UseInfo_Text->RemoveFromParent();
			StackSize_Text->RemoveFromParent();
		}
		ChangeColorByItemQuality(ItemData.GetQuality());
		break;

	default:
		break;
	}

	if (bTraderSlot)
	{
		Price_Text->SetText(FText::FromString(FString(FString::FromInt(ItemData.GetPurchase()))));
	}
	else
	{
		Price_Text->SetText(FText::FromString(FString(FString::FromInt(ItemData.GetSale()))));
	}
}

void UToolTipWidget::ChangeColorByItemQuality(EItemQuality ItemQuality)
{
	switch (ItemQuality)
	{
	case EItemQuality::Trash:
		Name_Text->SetColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f));

		Quality_Text->SetText(FText::FromString("Trash"));
		Quality_Text->SetColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f));
		break;
	case EItemQuality::Common:
		Quality_Text->SetText(FText::FromString("Common"));
		break;
	case EItemQuality::Magic:
		Name_Text->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
		Quality_Text->SetText(FText::FromString("Magic"));
		Quality_Text->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
		break;
	case EItemQuality::Rare:
		Name_Text->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 0.0f, 1.f));
		Quality_Text->SetText(FText::FromString("Rare"));
		Quality_Text->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f));
		break;
	case EItemQuality::Unique:
		Name_Text->SetColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
		Quality_Text->SetText(FText::FromString("Unique"));
		Quality_Text->SetColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
		break;
	case EItemQuality::legeond:
		Name_Text->SetColorAndOpacity(FLinearColor(1.0f, 0.1f, 0.0f, 1.0f));
		Quality_Text->SetText(FText::FromString("Legeond"));
		Quality_Text->SetColorAndOpacity(FLinearColor(1.0f, 0.1f, 0.0f, 1.0f));
		break;
	}
}