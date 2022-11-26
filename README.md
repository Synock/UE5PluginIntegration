# UE5PluginIntegration
This is the plugin integration demo project for different UE5 plugins

## Extending Components and Interfaces
Interfaces can be extended to retain the same internal mechanism in the plugin, but fit the needs of your specific project.

## Tutorial

Submodule are a great way to include plugins.

# Multiplayer Chat

Multiplayer chat plugin is available at https://github.com/Synock/UE5MultiplayerChatPlugin

See its included readme for more details on the general plugin workflow.

Note that in the custom HUD blueprint, the pointer to the chatbox are defined.

## PlayerController implementation of 

See the **_Chat_** section of the APluginIntegrationPlayerController

### General purpose functions
#### **_GetChatName_**
Here for simplicity purpose, we return a string initialized using the object's name.

#### **_IsInGroup_**
Is in group is taking a locally stored GroupId to check wether the player is in a group or not.

#### **_GetChatHUD_**
Simply return the locally stored HUD pointer.

### RPCs

All the needed RPC are redefined purely based on the basic proposed template in the plugin. It does the job well. The only reason why they need to be overriden on the project is because they must be considered RPCs?
* Client_AddChatData
* Client_AddChatDataType
* Server_AreaSpeak
* Server_ShoutSpeak
* Server_OOCSpeak
* Server_GroupSpeak
* Server_RaidSpeak
* Server_GuildSpeak
* Server_AuctionSpeak
* Server_TellSpeak

### Custom functions
A few custom functions have been implemented to make use of the chat plugin and interact with other plugins:

* AddQuestProgressChatData() Display a message when quest progress is logged.
* AddNewQuestChatData() Display a message when a new quest begin.
* AddItemLootNotification() Display a message when an item is looted.
* AddItemRewardNotification() Display a message when an item is rewarded through quest.
* GetMOTD() Display a "Message Of The Day" kind of message.

# Inventory

### Game mode interface (APluginIntegrationGameMode)

Your game modes should derive from IInventoryGameModeInterface.

### Character / Controller separation

Characters contains the components and the items, while Controller have the functions to equip/ swap / pay etc...

### Characters

#### Base character and equipment component

In the given project, a base character class has been defined. This base character class represent an humanoid actor that is capable of equipping stuff.

It is important that any actor capable of equipping items, derive from *IEquipmentInterface*.

*IEquipmentInterface* is the interface for equipping/removing items, it contains the following pure virtual functions.

    virtual UEquipmentComponent* GetEquipmentComponent() = 0;
    virtual const UEquipmentComponent* GetEquipmentComponentConst() const = 0;
    virtual AActor* GetEquipmentOwningActor() = 0;
    virtual AActor const* GetEquipmentOwningActorConst() const = 0;

You will need to implement all these functions, but the code given in *ACharacterBase* is sufficient.
You only need to define an *UEquipmentComponent* as one of the character actor component.
This component is initialized in the CharacterBase constructor and is replicated.
You will need to plug Calls from *ItemEquipedDispatcher_Server* and *ItemUnEquipedDispatcher_Server* to you equip/unequip function that handle stats changes.

This is the place where you should override *HandleEquipmentEffect(EEquipmentSlot InSlot, const FInventoryItem& LocalItem)*
 and *HandleUnEquipmentEffect(EEquipmentSlot InSlot, const FInventoryItem& LocalItem)* to take into account armor changes, or any other equipment related modifier.


#### Player Character, Inventory and complex coin management

Based of the *CharacterBase*, the main player character (*PluginIntegrationCharacter*) is extending its capability by adding inventory management and coin handling capabilities.
For this purpose, this class derive from :

    public ACharacterBase, public IInventoryInterface, public IPurseInterface

##### IInventoryInterface & UInventoryComponent

The interface is very simple and only require two pure virtual override.

    virtual UInventoryComponent* GetInventoryComponent() = 0;
	virtual const UInventoryComponent* GetInventoryComponentConst() const = 0;

These functions only send back the *UInventoryComponent*. All other Interface functions make use of these two functions, or internal functions can be accessed through these two.
See *APluginIntegrationCharacter* for an example.
Note that this component possess a *FullInventoryDispatcher_Server* delegate that is used in the example to trigger a server side weight update whenever inventory is modified.

##### IPurseInterface & UCoinComponent

The interface is also very simple and only require two pure virtual override.

    virtual UCoinComponent* GetPurseComponent() = 0;
	virtual const UCoinComponent* GetPurseComponentConst() const = 0;

These functions only send back the *UCoinComponent*  All other Interface functions make use of these two functions, or internal functions can be accessed through these two.
See *APluginIntegrationCharacter* for an example.
Note that this component possess a *PurseDispatcher_Server* delegate that is used in the example to trigger a server side weight update whenever the coin amount is modified.