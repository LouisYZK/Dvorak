"""
mock passengers' request
"""
# from .data_loader import *
from itertools import product
from os import sep
from numpy.core.defchararray import lower
import pandas as pd
import numpy as np
import random
from datetime import datetime, time, timedelta
from .data_loader import STOP_PAIR_MAP

class Order:
    def __init__(self,
                 id,
                 psg_num,
                 o_stop_id,
                 d_stop_id,
                 upper_time,
                 lower_time,
                 bus_id) -> None:
        self.id = id
        self.psg_num = psg_num
        self.o_stop_id = o_stop_id
        self.d_stop_id = d_stop_id
        self.upper_time = upper_time
        self.lower_time = lower_time
        self.bus_id = bus_id


class Bus:
    def __init__(self, 
                 id,
                 pos_lng,
                 pos_lat,
                 capacity,
                 order_list,
                 stop_list) -> None:
        self.id = id
        self.capacity = capacity
        self.order_list = order_list
        self.stop_list = stop_list

        self.pos_lng = pos_lng
        self.pos_lat = pos_lat

        self.stop_eta = []
        self.weight = 0

    def point_distance(self, stop_id):
        pass

    @property
    def total_distance(self):
        dist = 0
        try:
            for ind, stop in enumerate(self.stop_list):
                if ind == len(self.stop_list):
                    break
                dist += STOP_PAIR_MAP.get(
                    (stop.id, self.stop_list[ind+1])
                ).dist
        except ValueError:
            raise ValueError("No such stop pairs")
        return dist
            

class PsgRequest:
    def __init__(self, 
                 bus_list,
                 order_list) -> None:
        self.bus_list = bus_list
        self.order_list = order_list

    def to_str(self):
        for bus in self.bus_list:
            print("""
                Bus: %s, cap: %s, pos_lng: %s, pos_lat: %s
                stop list: %s
                order list: %s
            """ % (bus.id, bus.capacity, bus.pos_lng, bus.pos_lat,
                ','.join(bus.order_list), ','.join(bus.stop_list)))
        for order in self.order_list:
            print("""
                Order: %s, psg_num: %s, o_stop: %s, d_stop: %s
                upper_time: %s, lower_time: %s,
                bus_id: %s
            """ % (order.id, order.psg_num, order.o_stop_id,
                   order.d_stop_id, order.upper_time, order.lower_time,
                   order.bus_id))

def generate_resonale(line_id, order_num=5, bus_num=5):
    df_s = pd.read_csv('data/stops_%s.tsv' % line_id, header=None, sep='\t')
    # df_p = pd.read_csv('pairs_%s.tsv' % line_id, header=None, sep='\t')

    max_lng, min_lng = df_s.iloc[:, 2].max(), df_s.iloc[:, 2].min()
    max_lat, min_lat = df_s.iloc[:, 3].max(), df_s.iloc[:, 3].min()
    
    # mock orders
    stops = df_s.iloc[:, 0].unique().tolist()
    base_time = datetime(2021, 4, 1, 7, 0, 0 )
    order_list = []
    for ind in range(order_num):
        o, d = random.choices(stops, k=2)
        minutes = random.randint(1, 120)
        d = base_time + timedelta(minutes=minutes)
        lower_time = d.strftime("%Y-%m-%d %H:%M:%S")
        upper_time = ( d + timedelta(minutes=10)).strftime("%Y-%m-%d %H:%M:%S")
        psg_num = random.randint(1, 4)
        bus_id = 0 
        order_list.append(Order(ind, 
                psg_num, o, d, upper_time,
                lower_time, bus_id))

    # mock buses
    bus_list = []
    for ind in range(bus_num):
        lng = np.random.uniform(min_lng, max_lng)
        lat = np.random.uniform(min_lat, max_lat)
        
        bus_list.append(Bus(
            id=ind,
            pos_lng=lng,
            pos_lat=lat,
            capacity=12,
            order_list=[],
            stop_list=[]
        ))
    
    return order_list, bus_list


if __name__ == '__main__':
    orders, buses = generate_resonale('862')
    PsgRequest(buses, orders).to_str()    