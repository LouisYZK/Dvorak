import pandas as pd


stops_file = 'data/stops.tsv'
pair_file = 'data/pairs.tsv'
df_s = pd.read_csv(stops_file, sep='\t', header=None)
df_p = pd.read_csv(pair_file, sep='\t', header=None)


class Stop:
    def __init__(self, 
                 id: str,
                 name: str,
                 pos_lng,
                 pos_lat) -> None:
        self.id = id
        self.name = name
        self.pos_lng = pos_lng
        self.pos_lat = pos_lat
        
        self.bound_order = None

class StopPair:
    def __init__(self,
                 o_stop,
                 d_stop,
                 dist,
                 eta
                 ) -> None:
        self.o_stop = o_stop
        self.d_stop = d_stop
        self.dist = dist
        self.eta = eta


stop_ids = df_s.iloc[:, 0].tolist()
stop_names = df_s.iloc[:, 1].tolist()
lngs = df_s.iloc[:, 2].tolist()
lats = df_s.iloc[:, 3].tolist()

STOP_MAP = dict()
for id, n, lng, lat in zip(stop_ids, stop_names,
                lngs, lats):
    STOP_MAP[id] = Stop(id=id, name=n, pos_lng=lng, pos_lat=lat)

STOP_PAIR_MAP = dict()
o_stops = df_p.iloc[:, 0].tolist()
d_stops = df_p.iloc[:, 1].tolist()
dists = df_p.iloc[:, 2].tolist()
etas = df_p.iloc[:, 3].tolist()

for o, d, dist, eta in zip(o_stops, d_stops,
                            dists, etas):
    STOP_PAIR_MAP[(o, d)] = StopPair(o, d, dist, eta)


