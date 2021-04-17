import random
from numpy.lib import type_check
from .data_loader import STOP_MAP, STOP_PAIR_MAP
from .utils import naive_distance
from .mock_request import Bus, Order


def calculate_weight(bus: Bus):
    pass

def update_status(bus: Bus):
    """
    mainly update each stops eta
    """
    for ind, stop in enumerate(bus.stop_list):
        if ind == 0:
            bus.stop_eta[0] = bus.to_eta(stop.pos_lng, stop.pos_lat)
        pre_eta = sum(bus.stop_eta[:ind])
        bus.stop_eta[ind] = pre_eta + STOP_PAIR_MAP.get((
            bus.stop_list[ind-1].id, stop.id
        ))
    if len(bus.stop_list) < len(bus.stop_eta):
        bus.stop_eta[len(bus.stop_list)] = None

    ## update weight
    calculate_weight(bus)
        


def check(bus: Bus):
    assert len(bus.stop_list) == len(bus.stop_eta), "Stop Line Erorr!"

    # stop order checking
    for order in bus.order_list:
        ind_1 = bus.stop_list.find(order.o_stop)
        ind_2 = bus.stop_list.find(order.d_stop)
        if ind_1 == -1 or ind_2 == -1:
            return False
        if ind_1 > ind_2:
            return False

    # checking time window        
    for order in bus.order_list:
        stop_ind = bus.stop_list.find(order.o_stop)
        eta = bus.to_eta(bus.stop_list[stop_ind])
        
        if eta not in range(order.lower_time - 1.5, 
                            order.upper_time + 1.5):
            return False

    # checking capacity
    cap = sum([order.psg_num for order in bus.order_list ])
    if cap > bus.capacity:
        return False

    #checking detour ratio
    min_distance = 0
    for ind, stop in enumerate(bus.stop_list):
        if ind == len(bus.stop_list):
            break
        min_distance += STOP_PAIR_MAP.get(
            (stop, bus.stop_list[ind+1])
        ).dist
    real_distance = sum([eta for eta in bus.stop_eta])
    if real_distance / min_distance >= 1.5:
        return False

    return True
    

def insert_order(order: Order, bus: Bus):
    if bus.to_eta(order.o_stop_id) < order.upper_time and \
            bus.to_eta(order.d_stop_id) >= order.lower_time:
            bus.order_list.append(order.id)
            order.bus_id = bus.id
    update_status(bus)
    return check(bus)

def drop_order(bus):
    if len(bus.order_list) == 0:
        return False, None
    
    chosen_order = random.choice(bus.order_list)
    bus.stop_list.remove(chosen_order.o_stop)
    bus.stop_list.remove(chosen_order.d_stop)
    update_status(bus)
    print("Drop order %s" % chosen_order.id)
    return check(bus), chosen_order

def change_stops(bus):
    if len(bus.stop_list == 0):
        return False

    chosen_stop_index = random.randint(0, len(bus.stop_list))
    if chosen_stop_index != len(bus.stop_list):
        stop_1, stop_2 = bus.stop_list[chosen_stop_index], \
            bus.stop_list[chosen_stop_index + 1]
    else:
        stop_1, stop_2 = bus.stop_list[chosen_stop_index -1], \
            bus.stop_list[chosen_stop_index]
    if stop_1.order_id != stop_2.order_id:
        bus.stop_list[chosen_stop_index] = stop_2
        bus.stop_list[chosen_stop_index+1] = stop_1
        update_status(bus)
        return check(bus)
    else:
        return False


class BaseSolution:
    def __init__(self, bus_list, order_list) -> None:
        self.bus_list = bus_list
        self.order_list = order_list
        
        self.order_map = { order.id: order for order in order_list}

    def solve(self):
        for order in self.order_list:
            if order.bus_id != 0:
                continue
            for bus in self.bus_list:
                pass