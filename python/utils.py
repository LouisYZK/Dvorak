import math

def naive_distance(o_loc, d_loc):
    """
    Straight line euler distance and eta calculation
    """
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
    return s, s / ( 20 / 3.6)