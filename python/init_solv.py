from .data_loader import STOP_MAP
from .utils import naive_distance

def update_status(order_list, bus_list):
    order_map = { order.id: order for order in order_list }
    for bus in bus_list:
        pass


def insert_order(order, bus):
    pass


def drop_order(bus):
    pass


def change_stops(bus):
    pass


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