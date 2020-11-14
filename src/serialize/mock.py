"""
Mock Data Generation For serialization Tests;

data json type:
{
    id: int64,
    name: string,
    friends: int64[]
}
"""
import os
import random
import string
import json
import pickle
import time
from datetime import datetime

proj_path = os.path.abspath(os.path.join(
    os.getcwd(),
    '../..'
))
DATA_PATH = proj_path + '/data'
NUM = 1000

def generate():
    persons = []
    ids = []
    for ind in range(NUM):
        rand_id = random.randint( 1<<32, 1<<64 -1)
        name_len = random.randint(8, 16)
        rand_name = ''.join(random.sample(string.ascii_letters + string.digits, name_len))
        persons.append({
            'id': str(rand_id),
            'name': rand_name
        })
        ids.append(str(rand_id))
    # add friends randomly
    for per in persons:
        friends_num = random.randint(0, 6)
        friends_ids = random.choices(ids, k=friends_num)
        friends_ids = list(set(friends_ids))
        per['friends'] = friends_ids
    
    # store as json file for data source
    with open(DATA_PATH + "/person.json", 'w') as fp:
        data = {
            'date': datetime.now().strftime("%Y-%m-%d"),
            'data': persons
        }
        json.dump(data, fp)

    # test pickle in python
    start_dumps_time = time.time()
    with open(DATA_PATH + '/person.pkl', 'wb') as fp:
        pickle.dump(persons, fp)
    end_dumps_time = time.time()
    print("Pickling time: %d " % (end_dumps_time - start_dumps_time))

     # test pickle load in python
    start_loads_time = time.time()
    with open(DATA_PATH + '/person.pkl', 'rb') as fp:
        pickle.load(fp)
    end_loads_time = time.time()
    print("Pickling loading time: %d " % (end_loads_time - start_loads_time))


if __name__ == '__main__':
    generate()