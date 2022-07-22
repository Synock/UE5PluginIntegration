# UE5PluginIntegration
This is the plugin integration demo project for different UE5 plugins

### Extending Components and Interfaces
Interfaces can be extended to retain the same internal mechanism in the plugin, but fit the needs of your specific project.



## Tutorial

Submodule are a great way to include plugins.

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

These functions only send back the *UInventoryComponent* 