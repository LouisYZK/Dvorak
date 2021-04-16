"""
Request Bus Stop data & pair line data from Amap Open Platform
"""
import argparse
import requests
import math
from itertools import product
import json

city = 'xianyang'
# line_list = ['801', '802', '803', '820', '821', '822', '823', '824', '825', '826', '827', '828', '830', '831', '832', '833', '834', '837', '838', '839', '840', '841', '860', '861', '862', '863', '864', '865', '881快线881', '882', '883', '884', '1010', '1013', '1015', '1050', '1061', '1062', '1063', '1064', '1065', '1066', '1067', '1068', '1071', '1080', '1100', '1101', '1120', '1121', '1122', '1123', '1127', '1140', '1141', '1142', '1145', '1146', '1147', '1160', '1161', '1180', '1181', '1182', '1210', '1214', '1215', '1230']
line_list = ['862']


def naive_distance(o_loc, d_loc):
    x1, y1 = o_loc
    x2, y2 = d_loc
    y1 = float(y1); y2 = float(y2); x1 = float(x1); x2 = float(x2)
    
    rad_y1 = y1 * math.pi / 180
    rad_y2 = y2 * math.pi / 180
    a = rad_y1 - rad_y2
    b = x1 * math.pi / 180 - x2 * math.pi / 180
    s = 2 * math.asin(math.sqrt(math.pow(math.sin(a / 2), 2)
				+ math.cos(rad_y1) * math.cos(rad_y2)
				* math.pow(math.sin(b / 2), 2)))
    s = s * 6378137.0
    s = round(s * 10000) / 100000
    return s


def store_all(stop_list):
    f = open('stops_862.tsv', 'w')
    sid_dct = dict()
    for sid, name, loc in stop_list:
        if not sid:
            continue
        if sid not in sid_dct:
            f.write('%s\t%s\t%s\t%s\n' % (sid, name, 
                loc.split(',')[0],
                loc.split(',')[1]))
            sid_dct[sid] = (sid, name, loc)
    f.close()

    f = open('pairs_862.tsv', 'w')
    pair_dct = {}
    uniq_stops = sid_dct.keys()
    for o, d in product(uniq_stops, uniq_stops):
        if o != d:
            if (o, d) in uniq_stops:
                continue
            _, _, loc_o = sid_dct.get(o)
            _, _, loc_d = sid_dct.get(d)
            dist = naive_distance(loc_o.split(",")[:2],
                    loc_d.split(',')[:2])
            eta = dist / ( 20 / 3.6)
            pair_dct[(o, d)] = (dist, eta)
            pair_dct[(d, o)] = (dist, eta)
    for key, val in pair_dct.items():
        o, d = key
        dist, eta = val
        info = '%s\t%s\t%s\t%s\n' % (
            o, d, dist, eta
        )
        f.write(info)
    f.close()


def load(city, args):
    url = "https://restapi.amap.com/v3/bus/linename?parameters"
    all_stops = []
    for line_id in line_list:
        keyword = { "extensions": "all", 
                    "key": args.amap_key, 
                    "output": "json", 
                    "city": city, 
                    "offset": "0", 
                    "keywords": [line_id],
                    "platform": "JS"}
        res = requests.get(url, params=keyword).json()
        if res["status"] == "1":
            for line in res['buslines']:
                stops = [(item['id'], item['name'], item['location']) 
                            for item in line['busstops']]
                
                all_stops += stops
            print('get over...', line_id)
        else:
            print('request error!')

    store_all(all_stops)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--amap_key', '-a', type=str)
    args = parser.parse_args()
    load(city, args)