// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABGameInstance.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PJT_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UHUDOverlay> HUDOverlayAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UHUDOverlay* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UUserWidget* PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class USaveWindow> WSaveMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	USaveWindow* SaveMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> WDeadMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UUserWidget* DeadMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> WStageStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UUserWidget* StageStartWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> WStageClear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UUserWidget* StageClearWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> WGameClear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", Meta = (AllowPrivateAccess = true))
	UUserWidget* GameClearWidget;


	UPROPERTY()
	class UCharacterStatComponent* CharacterStat;

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;

	bool bInventoryWindow;
	bool bMenuWindow;
	bool bLootWindow;
	bool bSaveMenu;
	bool bTradeWindow;
	bool bTrainerWindow;

public:
	void OpenInventoryWindow();
	
	void OpenLootWindow(class AItemPot* TargetItemPot);
	void CloseLootWindow();
	
	UFUNCTION(BlueprintCallable)
	void OpenMenuWindow();
	
	void OpenSaveMenu();
	
	void OpenDeadMenu();
	
	void OpenTradeWindow(class ANpc* TradeTarget);
	void CloseTradeWindow();

	void OpenTrainerWindow();
	void CloseTrainerWindow();

	void RequestIncMoney(int32 IncMoney);

	UFUNCTION(BlueprintCallable)
	void CloseStageStartWidget();

	bool EquipItem(class UInventorySlot* EquippedItem);
	void EquipItemForName(FName ItemName);
	bool UnEquipItem(FName ItemName);

	void SaveGame();
	void NextStageSaveGame();
	void LoadGame();

	void RequestPurchaseItem(FName ItemName);

	float RequestPotion();

	void UpgradePotionAmount();
	void UpgradePotionForce();
	bool CheckUpEnoughMoney(int32 Needful);


	FORCEINLINE UHUDOverlay* GetHUDOverlay() { return HUDOverlay; }
	FORCEINLINE UCharacterStatComponent* GetCharacterStat() { return CharacterStat; }
	FORCEINLINE bool IsOpenLootWindow() { return bLootWindow; }
	FORCEINLINE bool IsOpenInventoryWindow() { return bInventoryWindow; }
	FORCEINLINE bool IsOpenMenuWindow() { return bMenuWindow; }
	FORCEINLINE bool IsOpenSaveMenu() { return bSaveMenu; }
	FORCEINLINE bool IsOpenTradeWindow() { return bTradeWindow; }
};
