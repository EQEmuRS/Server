/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2003 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

// @merth notes:
// These classes could be optimized with database reads/writes by storing
// a status flag indicating how object needs to interact with database

#ifndef __ITEM_H
#define __ITEM_H

class ItemInst;				// Item belonging to a client (contains info on item, dye, augments, charges, etc)
class ItemInstQueue;		// Queue of ItemInst objects (i.e., cursor)
class Inventory;			// Character inventory
class ItemParse;			// Parses item packets
class EvolveInfo;			// Stores information about an evolving item family

#include <string>
#include <vector>
#include <map>
#include <list>
#include "../common/eq_packet_structs.h"
#include "../common/eq_constants.h"
#include "../common/item_struct.h"
#include "../common/timer.h"


class ItemContainer;
class ItemInstance;
class ItemSlot;
class MobInventory;
class MobInventoryFactory;

typedef std::vector<ItemContainer*> icontainers;
typedef std::vector<ItemInstance*> ivector;
typedef std::list<ItemInstance*> ilist;

// need to create 
typedef std::list<ItemSlot_Struct> iresultlist;


//
// class ItemContainer
//
class ItemContainer
{
	friend class HasItemQuery;
	friend class MobInventory;
	friend class MobInventoryFactory;
	friend class Object;

public:
	~ItemContainer();

	void Initialize(size_t size);

	size_t Size() { return m_Bucket.size(); }
	size_t Count();
	bool Empty();
	
	const ItemInstance* InstanceAt(size_t index) const;
	const Item_Struct* ItemAt(size_t index) const;
	const Item_Struct* BaseItemAt(size_t index) const;

protected:
	ItemContainer(ItemContainer& itemContainer);
	ItemContainer(size_t size = 0) { Initialize(size); }

	void Resize(size_t size) { m_Bucket.resize(size); }
	void Concatenate();
	void ClearBucket();
	void ConsumeBucket();

	ivector& Bucket() { return m_Bucket; }

	ItemInstance* InstanceAt(size_t index);
	void AssignInstanceTo(size_t index, ItemInstance* itemInstance);
	ItemInstance* RemoveInstanceFrom(size_t index);
	void PopInstanceFrom(size_t index);
	void DeleteInstanceFrom(size_t index);

private:
	ivector m_Bucket;
};


//
// class ItemInstance
//
class ItemInstance
{
	friend class HasItemQuery;

public:

	uint32 GetSerialNumber() { return m_SerialNumber; }
	int16 GetCharges() { return m_Charges; }

	const Item_Struct* GetItemData() const { return m_ItemData; }

	bool IsClassType(uint8 itemClass) { return (m_ItemData->ItemClass == itemClass); }
	bool IsUseType(uint8 itemUse) { return (m_ItemData->ItemType == itemUse); }
	bool IsAugmentType(uint32 augmentType) { return (m_ItemData->AugType == augmentType); } // double-check if value is single- or multi-bit..may need bit-wise '&' (may need ItemClassCommon check)
	bool IsBagType(uint8 bagType) { return (IsClassType(ItemClassContainer) && (m_ItemData->BagType == bagType)); }

	uint32 GetItemID() { return m_ItemData->ID; }
	bool IsStackable() { return m_ItemData->Stackable; }

	bool IsTemporary() { return (m_ItemData->NoRent == 0); }
	bool IsTradeable() { return (m_ItemData->NoDrop != 0); }
	bool IsPetable() { return (m_ItemData->NoPet == 0); }
	bool IsTransferable() { return (m_ItemData->NoTransfer == 0); }
	//bool IsStorable() { return (m_ItemData->NoStorage == 0); } // not implemented yet
	
protected:

	ItemContainer* GetItemContainer() { return m_ItemContainer; }

private:
	int32 m_SerialNumber;
	int16 m_Charges;


	const Item_Struct* m_ItemData;


	ItemContainer* m_ItemContainer;
};


//
// class ItemSlot
//
class ItemSlot
{
public:
	// global methods
	static void Invalidate(ItemSlot_Struct& itemSlotStruct);
	static void Invalidate(ItemSlotShort_Struct& itemSlotStruct);

	static bool IsInvalid(const ItemSlot_Struct& itemSlotStruct);
	static bool IsInvalid(const ItemSlotShort_Struct& itemSlotStruct);
	static bool IsDelete(const ItemSlot_Struct& itemSlotStruct);
	static bool IsDelete(const ItemSlotShort_Struct& itemSlotStruct);
	static bool IsMain(const ItemSlot_Struct& itemSlotStruct);
	static bool IsMain(const ItemSlotShort_Struct& itemSlotStruct);
	static bool IsSub(const ItemSlot_Struct& itemSlotStruct);
	static bool IsSub(const ItemSlotShort_Struct& itemSlotStruct);
	static bool IsMainAug(const ItemSlot_Struct& itemSlotStruct);
	static bool IsMainAug(const ItemSlotShort_Struct& itemSlotStruct);
	static bool IsSubAug(const ItemSlot_Struct& itemSlotStruct);
	static bool IsSubAug(const ItemSlotShort_Struct& itemSlotStruct);

	static bool IsEqual(const ItemSlot_Struct& itemSlotStruct_1, const ItemSlot_Struct& itemSlotStruct_2);
	static bool IsEqual(const ItemSlot_Struct& itemSlotStruct_1, const ItemSlotShort_Struct& itemSlotStruct_2);
	static bool IsEqual(const ItemSlotShort_Struct& itemSlotStruct_1, const ItemSlot_Struct& itemSlotStruct_2);
	static bool IsEqual(const ItemSlotShort_Struct& itemSlotStruct_1, const ItemSlotShort_Struct& itemSlotStruct_2);
	static bool IsNotEqual(const ItemSlot_Struct& itemSlotStruct_1, const ItemSlot_Struct& itemSlotStruct_2);
	static bool IsNotEqual(const ItemSlot_Struct& itemSlotStruct_1, const ItemSlotShort_Struct& itemSlotStruct_2);
	static bool IsNotEqual(const ItemSlotShort_Struct& itemSlotStruct_1, const ItemSlot_Struct& itemSlotStruct_2);
	static bool IsNotEqual(const ItemSlotShort_Struct& itemSlotStruct_1, const ItemSlotShort_Struct& itemSlotStruct_2);

	static void GetOwningSlot(ItemSlot_Struct& owningSlotStruct, const ItemSlot_Struct& itemSlotStruct);
	static void GetOwningSlot(ItemSlotShort_Struct& owningSlotStruct, const ItemSlotShort_Struct& itemSlotStruct);

	static void GetTopSlot(ItemSlot_Struct& topSlotStruct, const ItemSlot_Struct& itemSlotStruct);
	static void GetTopSlot(ItemSlotShort_Struct& topSlotStruct, const ItemSlotShort_Struct& itemSlotStruct);

	static void CopySlot(ItemSlot_Struct& copySlotStruct, const ItemSlot_Struct& itemSlotStruct);
	static void CopySlot(ItemSlotShort_Struct& copySlotStruct, const ItemSlotShort_Struct& itemSlotStruct);

	// local methods
	void Invalidate() { ItemSlot::Invalidate(m_ItemSlotStruct); }

	bool IsInvalid() { return ItemSlot::IsInvalid(m_ItemSlotStruct); }
	bool IsDelete() { return ItemSlot::IsDelete(m_ItemSlotStruct); }
	bool IsMain() { return ItemSlot::IsMain(m_ItemSlotStruct); }
	bool IsSub() { return ItemSlot::IsSub(m_ItemSlotStruct); }
	bool IsMainAug() { return ItemSlot::IsMainAug(m_ItemSlotStruct); }
	bool IsSubAug() { return ItemSlot::IsSubAug(m_ItemSlotStruct); }

	bool IsEqual(const ItemSlot_Struct& itemSlotStruct) { return ItemSlot::IsEqual(m_ItemSlotStruct, itemSlotStruct); }
	bool IsEqual(const ItemSlotShort_Struct& itemSlotStruct) { return ItemSlot::IsEqual(m_ItemSlotStruct, itemSlotStruct); }
	bool IsNotEqual(const ItemSlot_Struct& itemSlotStruct) { return ItemSlot::IsNotEqual(m_ItemSlotStruct, itemSlotStruct); }
	bool IsNotEqual(const ItemSlotShort_Struct& itemSlotStruct) { return ItemSlot::IsNotEqual(m_ItemSlotStruct, itemSlotStruct); }

	void GetOwningSlot(ItemSlot_Struct& owningSlotStruct);
	void GetOwningSlot(ItemSlotShort_Struct& owningSlotStruct);

	void GetTopSlot(ItemSlot_Struct& topSlotStruct);
	void GetTopSlot(ItemSlotShort_Struct& topSlotStruct);

	void GetItemSlot(ItemSlot_Struct& getSlotStruct);
	void GetItemSlot(ItemSlotShort_Struct& getSlotStruct);
	void SetItemSlot(const ItemSlot_Struct& itemSlotStruct);
	void SetItemSlot(const ItemSlotShort_Struct& itemSlotStruct);

	void operator = (const ItemSlot_Struct& itemSlotStruct) { SetItemSlot(itemSlotStruct); }
	void operator = (const ItemSlotShort_Struct& itemSlotStruct) { SetItemSlot(itemSlotStruct); }
	bool operator == (const ItemSlot_Struct& itemSlotStruct) { return IsEqual(itemSlotStruct); }
	bool operator == (const ItemSlotShort_Struct& itemSlotStruct) { return IsEqual(itemSlotStruct); }
	bool operator != (const ItemSlot_Struct& itemSlotStruct) { return IsNotEqual(itemSlotStruct); }
	bool operator != (const ItemSlotShort_Struct& itemSlotStruct) { return IsNotEqual(itemSlotStruct); }

private:
	ItemSlot_Struct m_ItemSlotStruct;
};


enum HIQueryTypes
{
	HIQTNone = 0,
	HIQTExtant,
	HIQTPointer,
	HIQTSerialNumber,
	HIQTItemID,
	HIQTLore,
	HIQTLoreGroup,
	HIQTTemporary,
	HIQTNotTemporary,
	HIQTTradeable,
	HIQTNotTradeable,
	HIQTPetable,
	HIQTNotPetable,
	HIQTTransferable,
	HIQTNotTransferable,
	HIQTStorable,
	HIQTNotStorable,
	HIQTItemClass,
	HIQTItemUse,
	HIQTAugmentType,
	HIQTBagType
};


//
// class HasItemQuery
//
class HasItemQuery
{
public:
	HasItemQuery() { ResetCriteria(); }
	~HasItemQuery();

	void ResetCriteria();

	void SetQuantity(size_t queryQuantity) { m_Quantity = queryQuantity; }
	size_t QuantityFound() { return m_QuantityFound; }

	// full-scope methods
	void SetInventory(MobInventory* queryInventory) { m_Inventory = queryInventory; }

	void FlagMap(InventoryMapTypes mapIndex);
	void FlagEquipment();
	void FlagGeneral();
	void FlagCursor();
	void FlagDefault();
	void IgnoreMain() { m_IgnoreMain = true; }
	void IgnoreSub() { m_IgnoreSub = true; }
	void IgnoreMainAug() { m_IgnoreMainAug = true; }
	void IgnoreSubAug() { m_IgnoreSubAug = true; }

	void Execute();

	iresultlist* ResultList() { return &m_ResultList; }
	iresultlist::iterator begin() { return m_ResultList.begin(); }
	iresultlist::iterator end() { return m_ResultList.end(); }

	bool Found() { return m_Found; }
	size_t Count() { return m_ResultList.size(); }
	void Clear() { m_ResultList.clear(); }

	// limited-scope methods
	void SetContainer(ItemContainer* queryContainer) { m_Container = queryContainer; }
	void SetContainerSize(size_t queryContainerSize) { m_ContainerSize = queryContainerSize; }

	void ExecuteContainer();

	std::list<int16>* ContainerResultList() { return &m_ContainerResultList; }
	std::list<int16>::iterator container_begin() { return m_ContainerResultList.begin(); }
	std::list<int16>::iterator container_end() { return m_ContainerResultList.end(); }

	bool ContainerFound() { return m_ContainerFound; }
	size_t ContainerCount() { return m_ContainerResultList.size(); }
	void ContainerClear() { m_ContainerResultList.clear(); }

	// common criteria methods
	void SetType(HIQueryTypes queryType) { m_Type = queryType; }

	// TODO: add additional flag and criteria accessors

	// fields that have a valid entry of '0' use an offset that is removed during the query..and restored afterwards
	void SetItemID(uint32 itemID) { m_ItemID = itemID; }
	void SetLoreGroup(uint32 loreGroup) { m_LoreGroup = loreGroup; }
	void SetItemClass(uint8 itemClass) { m_ItemClass = (itemClass + 1); }
	void SetItemUse(uint8 itemUse) { m_ItemUse = (itemUse + 1); }
	void SetAugmentType(uint32 augmentType) { m_AugmentType = m_AugmentType; }
	void SetBagType(uint8 bagType) { m_BagType = (bagType + 1); }

private:
	void execute_();
	void execute_container_();
	bool validate_criteria_();
	void restore_criteria_();
	bool check_criteria_(ItemInstance* testInstance);

	MobInventory* m_Inventory;
	ItemContainer* m_Container;
	size_t m_ContainerSize;

	bool m_Locations[_MapCount];
	bool m_Equipment;
	bool m_General;
	bool m_Cursor;
	bool m_LocationSet;
	bool m_IgnoreMain;
	bool m_IgnoreSub;
	bool m_IgnoreMainAug;
	bool m_IgnoreSubAug;

	HIQueryTypes m_Type;
	size_t m_Quantity;
	size_t m_QuantityFound;
	ItemSlot_Struct m_SlotFound;

	ItemInstance* m_Pointer;
	uint32 m_SerialNumber;
	uint32 m_ItemID;
	uint32 m_LoreGroup;

	uint8 m_ItemClass;
	uint8 m_ItemUse;
	uint32 m_AugmentType;
	uint8 m_BagType;

	// TODO: add additional flag and criteria properties

	iresultlist m_ResultList;
	iresultlist* m_ExternalList;
	bool m_Found;

	std::list<int16> m_ContainerResultList;
	std::list<int16> m_ContainerWorkingList;
	bool m_ContainerFound;
};


//
// class MobInventory
//
class MobInventory
{
	friend class MobInventoryFactory;
	friend class HasItemQuery;

public:
	static void MarkDirty(ItemInstance* itemInstance);
	static void CleanDirty();

	~MobInventory();

	uint16 GetMapSize(int16 mapIndex);
	uint64 GetPossessionsBitmask() { return m_PossessionsBitmask; }
	uint64 GetEquipmentBitmask() { return m_EquipmentBitmask; }
	uint64 GetGeneralBitmask() { return m_GeneralBitmask; }
	uint64 GetCursorBitmask() { return m_CursorBitmask; }
	uint16 GetItemClassSize(ItemClassTypes itemClass);

protected:
	MobInventory();

	ItemContainer* GetMapContainer(int16 mapIndex);

private:
	uint16 m_MapSize[_MapCount];
	uint64 m_PossessionsBitmask;
	uint64 m_EquipmentBitmask;
	uint64 m_GeneralBitmask;
	uint64 m_CursorBitmask;

	uint16 m_ItemClassSize[_ItemClassCount];

	icontainers m_Containers;

	bool m_ConcatenateLimbo;

	ItemSlot m_ItemSlot;

	EQClientVersion m_Version;
};


//
// class MobInventoryFactory
//
class MobInventoryFactory
{
public:
	static MobInventory* GenerateInventory(EQClientVersion version = EQClientUnknown);
	static void ReconfigureInventory(MobInventory* inventoryInstance, EQClientVersion inventoryVersion);
};


// ************************
//
// inventory v2 demarcation
//
// ************************


// Helper typedefs
typedef std::list<ItemInst*>::const_iterator				iter_queue;
typedef std::map<int16, ItemInst*>::const_iterator			iter_inst;
typedef std::map<uint8, ItemInst*>::const_iterator			iter_contents;

namespace ItemField
{
	enum
	{
		source = 0,
#define F(x) x,
#include "item_fieldlist.h"
#undef F
		updated
	};
};

// Specifies usage type for item inside ItemInst
enum ItemInstTypes
{
	ItemInstNormal = 0,
	ItemInstWorldContainer
};

typedef enum {
	byFlagIgnore,	//do not consider this flag
	byFlagSet,		//apply action if the flag is set
	byFlagNotSet	//apply action if the flag is NOT set
} byFlagSetting;


//FatherNitwit: location bits for searching specific
//places with HasItem() and HasItemByUse()
enum {
	invWhereWorn		= 0x01,
	invWherePersonal	= 0x02,	//in the character's inventory
	invWhereBank		= 0x04,
	invWhereSharedBank	= 0x08,
	invWhereTrading		= 0x10,
	invWhereCursor		= 0x20
};


// ########################################
// Class: Queue
//	Queue that allows a read-only iterator
class ItemInstQueue
{
public:
	~ItemInstQueue();
	/////////////////////////
	// Public Methods
	/////////////////////////

	inline iter_queue begin()	{ return m_list.begin(); }
	inline iter_queue end()		{ return m_list.end(); }

	void push(ItemInst* inst);
	void push_front(ItemInst* inst);
	ItemInst* pop();
	ItemInst* peek_front() const;
	inline int size()		{ return static_cast<int>(m_list.size()); }

protected:
	/////////////////////////
	// Protected Members
	/////////////////////////

	std::list<ItemInst*> m_list;

};

// ########################################
// Class: Inventory
//	Character inventory
class Inventory
{
	friend class ItemInst;
public:
	///////////////////////////////
	// Public Methods
	///////////////////////////////

	Inventory() { m_version = EQClientUnknown; m_versionset = false; }
	~Inventory();

	// Inventory v2 creep
	bool SetInventoryVersion(EQClientVersion version) {
		if (!m_versionset) {
			m_version = version;
			return (m_versionset = true);
		}
		else {
			return false;
		}
	}

	EQClientVersion GetInventoryVersion() { return m_version; }

	static void CleanDirty();
	static void MarkDirty(ItemInst *inst);

	// Retrieve a writeable item at specified slot
	ItemInst* GetItem(int16 slot_id) const;
	ItemInst* GetItem(int16 slot_id, uint8 bagidx) const;

	inline iter_queue cursor_begin()	{ return m_cursor.begin(); }
	inline iter_queue cursor_end()		{ return m_cursor.end(); }
	inline bool CursorEmpty()			{ return (m_cursor.size() == 0); }

	// Retrieve a read-only item from inventory
	inline const ItemInst* operator[](int16 slot_id) const { return GetItem(slot_id); }

	// Add item to inventory
	int16 PutItem(int16 slot_id, const ItemInst& inst);

	// Add item to cursor queue
	int16 PushCursor(const ItemInst& inst);

	// Swap items in inventory
	bool SwapItem(int16 slot_a, int16 slot_b);

	// Remove item from inventory
	bool DeleteItem(int16 slot_id, uint8 quantity=0);

	// Checks All items in a bag for No Drop
	bool CheckNoDrop(int16 slot_id);

	// Remove item from inventory (and take control of memory)
	ItemInst* PopItem(int16 slot_id);

	// Check whether there is space for the specified number of the specified item.
	bool HasSpaceForItem(const Item_Struct *ItemToTry, int16 Quantity);

	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	int16 HasItem(uint32 item_id, uint8 quantity = 0, uint8 where = 0xFF);

	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	int16 HasItemByUse(uint8 use, uint8 quantity=0, uint8 where=0xFF);

	// Check whether item exists in inventory
	// where argument specifies OR'd list of invWhere constants to look
	int16 HasItemByLoreGroup(uint32 loregroup, uint8 where=0xFF);

	// Locate an available inventory slot
	int16 FindFreeSlot(bool for_bag, bool try_cursor, uint8 min_size = 0, bool is_arrow = false);
	int16 FindFreeSlotForTradeItem(const ItemInst* inst);

	// Calculate slot_id for an item within a bag
	static int16 CalcSlotId(int16 slot_id); // Calc parent bag's slot_id
	static int16 CalcSlotId(int16 bagslot_id, uint8 bagidx); // Calc slot_id for item inside bag
	static uint8 CalcBagIdx(int16 slot_id); // Calc bagidx for slot_id
	static int16 CalcSlotFromMaterial(uint8 material);
	static uint8 CalcMaterialFromSlot(int16 equipslot);

	static bool CanItemFitInContainer(const Item_Struct *ItemToTry, const Item_Struct *Container);

	//  Test for valid inventory casting slot
	bool SupportsClickCasting(int16 slot_id);
	bool SupportsPotionBeltCasting(int16 slot_id);

	// Test whether a given slot can support a container item
	static bool SupportsContainers(int16 slot_id);

	int GetSlotByItemInst(ItemInst *inst);

	void dumpEntireInventory();
	void dumpWornItems();
	void dumpInventory();
	void dumpBankItems();
	void dumpSharedBankItems();

	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, std::string value);
	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, int value);
	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, float value);
	void SetCustomItemData(uint32 character_id, int16 slot_id, std::string identifier, bool value);
	std::string GetCustomItemData(int16 slot_id, std::string identifier);
protected:
	///////////////////////////////
	// Protected Methods
	///////////////////////////////

	int GetSlotByItemInstCollection(const std::map<int16, ItemInst*> &collection, ItemInst *inst);
	void dumpItemCollection(const std::map<int16, ItemInst*> &collection);
	void dumpBagContents(ItemInst *inst, iter_inst *it);

	// Retrieves item within an inventory bucket
	ItemInst* _GetItem(const std::map<int16, ItemInst*>& bucket, int16 slot_id) const;

	// Private "put" item into bucket, without regard for what is currently in bucket
	int16 _PutItem(int16 slot_id, ItemInst* inst);

	// Checks an inventory bucket for a particular item
	int16 _HasItem(std::map<int16, ItemInst*>& bucket, uint32 item_id, uint8 quantity);
	int16 _HasItem(ItemInstQueue& iqueue, uint32 item_id, uint8 quantity);
	int16 _HasItemByUse(std::map<int16, ItemInst*>& bucket, uint8 use, uint8 quantity);
	int16 _HasItemByUse(ItemInstQueue& iqueue, uint8 use, uint8 quantity);
	int16 _HasItemByLoreGroup(std::map<int16, ItemInst*>& bucket, uint32 loregroup);
	int16 _HasItemByLoreGroup(ItemInstQueue& iqueue, uint32 loregroup);


	// Player inventory
	std::map<int16, ItemInst*>	m_worn;		// Items worn by character
	std::map<int16, ItemInst*>	m_inv;		// Items in character personal inventory
	std::map<int16, ItemInst*>	m_bank;		// Items in character bank
	std::map<int16, ItemInst*>	m_shbank;	// Items in character shared bank
	std::map<int16, ItemInst*>	m_trade;	// Items in a trade session
	ItemInstQueue				m_cursor;	// Items on cursor: FIFO

private:
	// Active inventory version
	EQClientVersion m_version;
	bool m_versionset;
};

class SharedDatabase;

// ########################################
// Class: ItemInst
//	Base class for an instance of an item
//	An item instance encapsulates item data + data specific
//	to an item instance (includes dye, augments, charges, etc)
class ItemInst
{
public:
	/////////////////////////
	// Methods
	/////////////////////////

	// Constructors/Destructor
	ItemInst(const Item_Struct* item = nullptr, int16 charges = 0);

	ItemInst(SharedDatabase *db, uint32 item_id, int16 charges = 0);

	ItemInst(ItemInstTypes use_type);

	ItemInst(const ItemInst& copy);

	~ItemInst();

	// Query item type
	bool IsType(ItemClassTypes item_class) const;

	// Can item be stacked?
	bool IsStackable() const;
	bool IsCharged() const;

	// Can item be equipped by/at?
	bool IsEquipable(uint16 race, uint16 class_) const;
	bool IsEquipable(int16 slot_id) const;

	//
	// Augements
	//
	inline bool IsAugmentable() const { return m_item->AugSlotType[0]!=0 || m_item->AugSlotType[1]!=0 || m_item->AugSlotType[2]!=0 || m_item->AugSlotType[3]!=0 || m_item->AugSlotType[4]!=0; }
	bool AvailableWearSlot(uint32 aug_wear_slots) const;
	int8 AvailableAugmentSlot(int32 augtype) const;
	bool IsAugmentSlotAvailable(int32 augtype, uint8 slot) const;
	inline int32 GetAugmentType() const { return m_item->AugType; }

	inline bool IsExpendable() const { return ((m_item->Click.Type == ET_Expendable ) || (m_item->ItemType == ItemTypePotion)); }

	//
	// Contents
	//
	ItemInst* GetItem(uint8 slot) const;
	uint32 GetItemID(uint8 slot) const;
	inline const ItemInst* operator[](uint8 slot) const { return GetItem(slot); }
	void PutItem(uint8 slot, const ItemInst& inst);
	void PutItem(SharedDatabase *db, uint8 slot, uint32 item_id) { return; } // not defined anywhere...
	void DeleteItem(uint8 slot);
	ItemInst* PopItem(uint8 index);
	void Clear();
	void ClearByFlags(byFlagSetting is_nodrop, byFlagSetting is_norent);
	uint8 FirstOpenSlot() const;
	uint8 GetTotalItemCount() const;
	bool IsNoneEmptyContainer();
	std::map<uint8, ItemInst*>* GetContents() { return &m_contents; }

	//
	// Augments
	//
	ItemInst* GetAugment(uint8 slot) const;
	uint32 GetAugmentItemID(uint8 slot) const;
	void PutAugment(uint8 slot, const ItemInst& inst);
	void PutAugment(SharedDatabase *db, uint8 slot, uint32 item_id);
	void DeleteAugment(uint8 slot);
	ItemInst* RemoveAugment(uint8 index);
	bool IsAugmented();
	ItemInst* GetOrnamentationAug(int ornamentationAugtype) const;
	static bool CanTransform(const Item_Struct *ItemToTry, const Item_Struct *Container, bool AllowAll = false);
	
	// Has attack/delay?
	bool IsWeapon() const;
	bool IsAmmo() const;

	// Accessors
	const uint32 GetID() const { return m_item->ID; }
	const uint32 GetItemScriptID() const { return m_item->ScriptFileID; }
	const Item_Struct* GetItem() const;
	const Item_Struct* GetUnscaledItem() const;

	int16 GetCharges() const				{ return m_charges; }
	void SetCharges(int16 charges)			{ m_charges = charges; }

	uint32 GetPrice() const					{ return m_price; }
	void SetPrice(uint32 price)				{ m_price = price; }

	void SetColor(uint32 color)				{ m_color = color; }
	uint32 GetColor() const					{ return m_color; }

	uint32 GetMerchantSlot() const				{ return m_merchantslot; }
	void SetMerchantSlot(uint32 slot)		{ m_merchantslot = slot; }

	int32 GetMerchantCount() const				{ return m_merchantcount; }
	void SetMerchantCount(int32 count)		{ m_merchantcount = count; }

	int16 GetCurrentSlot() const			{ return m_currentslot; }
	void SetCurrentSlot(int16 curr_slot)	{ m_currentslot = curr_slot; }

	// Is this item already attuned?
	bool IsInstNoDrop() const { return m_instnodrop; }
	void SetInstNoDrop(bool flag) { m_instnodrop=flag; }

	std::string GetCustomDataString() const;
	std::string GetCustomData(std::string identifier);
	void SetCustomData(std::string identifier, std::string value);
	void SetCustomData(std::string identifier, int value);
	void SetCustomData(std::string identifier, float value);
	void SetCustomData(std::string identifier, bool value);
	void DeleteCustomData(std::string identifier);

	// Allows treatment of this object as though it were a pointer to m_item
	operator bool() const { return (m_item != nullptr); }

	// Compare inner Item_Struct of two ItemInst objects
	bool operator==(const ItemInst& right) const { return (this->m_item == right.m_item); }
	bool operator!=(const ItemInst& right) const { return (this->m_item != right.m_item); }

	// Clone current item
	ItemInst* Clone() const;

	bool IsSlotAllowed(int16 slot_id) const;

	bool IsScaling() const				{ return m_scaling; }
	bool IsEvolving() const				{ return (m_evolveLvl >= 1); }
	uint32 GetExp() const				{ return m_exp; }
	void SetExp(uint32 exp)				{ m_exp = exp; }
	void AddExp(uint32 exp)				{ m_exp += exp; }
	bool IsActivated()					{ return m_activated; }
	void SetActivated(bool activated)	{ m_activated = activated; }
	int8 GetEvolveLvl() const			{ return m_evolveLvl; }
	void SetScaling(bool v)				{ m_scaling = v; }
	uint32 GetOrnamentationIcon() const					{ return m_ornamenticon; }
	void SetOrnamentIcon(uint32 ornament_icon)  		{ m_ornamenticon = ornament_icon; }
	uint32 GetOrnamentationIDFile() const 				{ return m_ornamentidfile; }
	void SetOrnamentationIDFile(uint32 ornament_idfile) { m_ornamentidfile = ornament_idfile; }

	void Initialize(SharedDatabase *db = nullptr);
	void ScaleItem();
	bool EvolveOnAllKills() const;
	int8 GetMaxEvolveLvl() const;
	uint32 GetKillsNeeded(uint8 currentlevel);

	std::string Serialize(int16 slot_id) const { InternalSerializedItem_Struct s; s.slot_id=slot_id; s.inst=(const void *)this; std::string ser; ser.assign((char *)&s,sizeof(InternalSerializedItem_Struct)); return ser; }
	inline int32 GetSerialNumber() const { return m_SerialNumber; }
	inline void SetSerialNumber(int32 id) { m_SerialNumber = id; }

	std::map<std::string, Timer>& GetTimers() { return m_timers; }
	void SetTimer(std::string name, uint32 time);
	void StopTimer(std::string name);
	void ClearTimers();

protected:
	//////////////////////////
	// Protected Members
	//////////////////////////
	iter_contents _begin()		{ return m_contents.begin(); }
	iter_contents _end()		{ return m_contents.end(); }

	friend class Inventory;


	void _PutItem(uint8 index, ItemInst* inst) { m_contents[index] = inst; }

	ItemInstTypes		m_use_type;	// Usage type for item
	const Item_Struct*	m_item;		// Ptr to item data
	int16				m_charges;	// # of charges for chargeable items
	uint32				m_price;	// Bazaar /trader price
	uint32				m_color;
	uint32				m_merchantslot;
	int16				m_currentslot;
	bool				m_instnodrop;
	int32				m_merchantcount;		//number avaliable on the merchant, -1=unlimited
	int32				m_SerialNumber;	// Unique identifier for this instance of an item. Needed for Bazaar.
	uint32				m_exp;
	int8				m_evolveLvl;
	bool				m_activated;
	Item_Struct*		m_scaledItem;
	EvolveInfo*			m_evolveInfo;
	bool				m_scaling;
	uint32				m_ornamenticon;
	uint32				m_ornamentidfile;

	//
	// Items inside of this item (augs or contents);
	std::map<uint8, ItemInst*>			m_contents; // Zero-based index: min=0, max=9
	std::map<std::string, std::string>	m_custom_data;
	std::map<std::string, Timer>		m_timers;
};

class EvolveInfo {
public:
	friend class ItemInst;
	//temporary
	uint16				LvlKills[9];
	uint32				FirstItem;
	uint8				MaxLvl;
	bool				AllKills;

	EvolveInfo();
	EvolveInfo(uint32 first, uint8 max, bool allkills, uint32 L2, uint32 L3, uint32 L4, uint32 L5, uint32 L6, uint32 L7, uint32 L8, uint32 L9, uint32 L10);
	~EvolveInfo();
};

#endif // #define __ITEM_H
