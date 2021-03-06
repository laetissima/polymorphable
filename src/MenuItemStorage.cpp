/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class MenuItemStorage
 */

#include "InputState.h"
#include "MenuItemStorage.h"

using namespace std;

void MenuItemStorage::init(int _slot_number, ItemManager *_items, SDL_Rect _area, int _icon_size, int _nb_cols) {
	ItemStorage::init( _slot_number, _items);
	area.push_back(_area);
	icon_size = _icon_size;
	nb_cols = _nb_cols;
	drag_prev_slot = -1;
}

/**
 * Overloaded function for case, if slot positions are predefined
 */
void MenuItemStorage::init(int _slot_number, ItemManager *_items, vector<SDL_Rect> _area, vector<string> _slot_type) {
	ItemStorage::init( _slot_number, _items);
	area = _area;
	icon_size = 0;
	nb_cols = 0;
	slot_type = _slot_type;
	drag_prev_slot = -1;
}

void MenuItemStorage::render() {
	for (int i=0; i<slot_number; i++) {
		if (storage[i].item > 0 && nb_cols > 0) {
			items->renderIcon(storage[i], area[0].x + (i % nb_cols * icon_size), area[0].y + (i / nb_cols * icon_size), icon_size);
		} else if (storage[i].item > 0 && nb_cols == 0) {
			items->renderIcon(storage[i], area[i].x, area[i].y, area[i].w);
		}
	}
}

int MenuItemStorage::slotOver(Point mouse) {
	if (isWithin(area[0], mouse) && nb_cols > 0) {
		return (mouse.x - area[0].x) / icon_size + (mouse.y - area[0].y) / icon_size * nb_cols;
	}
	else if (nb_cols == 0) {
		for (unsigned int i=0; i<area.size(); i++) {
			if (isWithin(area[i], mouse)) return i;
		}
	}
	return -1;
}

TooltipData MenuItemStorage::checkTooltip(Point mouse, StatBlock *stats, bool vendor_view) {
	TooltipData tip;
	int slot = slotOver( mouse);

	if (slot > -1 && storage[slot].item > 0) {
		return items->getTooltip( storage[slot].item, stats, vendor_view);
	}
	return tip;
}

ItemStack MenuItemStorage::click(InputState * input) {
	ItemStack item;
	drag_prev_slot = slotOver(input->mouse);
	if( drag_prev_slot > -1) { 
		item = storage[drag_prev_slot];
		if( input->pressing[SHIFT]) {
			item.quantity = 1;
		}
		substract( drag_prev_slot, item.quantity);
		return item;
	}
	else {
		item.item = 0;
		item.quantity = 0;
		return item;
	}
}

void MenuItemStorage::itemReturn(ItemStack stack) {
	add( stack, drag_prev_slot);
	drag_prev_slot = -1;
}

/**
 * Sort storage array, so items order matches slots order
 */
void MenuItemStorage::fillEquipmentSlots() {
	// create temporary arrays
	int *equip_item = new int[slot_number];
	int *equip_quantity = new int[slot_number];;

	// initialize arrays
	for (int i=0; i<slot_number; i++) {
		equip_item[i] = storage[i].item;
		equip_quantity[i] = storage[i].quantity;
	}
	// clean up storage[]
	for (int i=0; i<slot_number; i++) {
		storage[i].item = 0;
		storage[i].quantity = 0;
	}

	// fill slots with items
	for (int i=0; i<slot_number; i++) {
		for (int j=0; j<slot_number; j++) {
			// search for empty slot with needed type. If item is not NULL, put it there
			if (items->items[equip_item[i]].type == slot_type[j] && equip_item[i] > 0 && storage[j].item == 0) {
				storage[j].item = equip_item[i];
				storage[j].quantity = equip_quantity[i];
				break;
			}
		}
	}
	delete [] equip_item;
	delete [] equip_quantity;
}

