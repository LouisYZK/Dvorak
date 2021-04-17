from hashlib import new
from os import access
import random
import math
from .init_solv import (
    insert_order,
    calculate_weight,
    drop_order,
    change_stops,
    update_status,
    check
)
from .data_loader import STOP_PAIR_MAP

INIT_TEMP = 100
DECAY_RATIO = 0.1

class SAPlanSovling:
    def __init__(self, bus_list, order_list) -> None:
        self.bus_list = bus_list
        self.order_list = order_list

        self.empty_order = [order for order in self.order_list if order.bus_id == 0]

    @property
    def total_weight(self):
        return sum([
            calculate_weight(bus) \
                for bus in self.bus_list
        ])

    def solve(self):
        init_w = self.total_weight
        glob_w = self.total_weight
        while True:
            choice_action = random.choice(['swap', 'drop', 'insert'])
            if choice_action == 'swap':
                bus = random.choice(self.bus_list)
                change_stops(bus)
                temp_bus = update_status(bus)
                if check(temp_bus):
                    new_w = calculate_weight(bus)
                    if new_w > glob_w:
                        glob_w = new_w
                        bus = temp_bus
                    else:
                        delta = new_w - init_w
                        prob = random.uniform(0, 1)
                        if prob < math.exp(- delta / INIT_TEMP):
                            bus = temp_bus
                            init_w = new_w
                else:
                    INIT_TEMP *= DECAY_RATIO

            if choice_action == 'insert':
                if len(self.empty_order) == 0:
                    continue
                order = random.choice(self.empty_order)
                bus = random.choice(self.bus_list)
                temp_bus = insert_order(order, bus)
                update_status(bus)
                if check(bus):
                    new_w = calculate_weight(temp_bus)
                    if new_w > glob_w:
                        glob_w = new_w
                        bus = temp_bus
                        order.bus_id = bus.id
                        self.order_list.remove(order)
                    else:
                        delta = new_w - init_w
                        prob = random.uniform(0, 1)
                        if prob < math.exp(- delta / INIT_TEMP):
                            bus = temp_bus
                            init_w = new_w
                else:
                    INIT_TEMP * DECAY_RATIO

            if choice_action == 'drop':
                bus = random.choice(self.bus_list)
                order = random.choice(bus.order_list)
                temp_bus = drop_order(order, bus)
                update_status(bus)
                if check(bus):
                    new_w = calculate_weight(temp_bus)
                    if new_w > glob_w:
                        glob_w = new_w
                        bus = temp_bus
                        order.bus_id = bus.id
                        self.order_list.append(order)
                    else:
                        delta = new_w - init_w
                        prob = random.uniform(0, 1)
                        if prob < math.exp(- delta / INIT_TEMP):
                            bus = temp_bus
                            init_w = new_w
                else:
                    INIT_TEMP * DECAY_RATIO
            if INIT_TEMP < 0.01:
                break
        