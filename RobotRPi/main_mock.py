import random
import time
from api_posting import MeasPoster


mp = MeasPoster(1)

while True:
    mp.publish(random.randrange(15, 25), random.randrange(0, 100), bool(random.getrandbits(1)))
    time.sleep(1)
