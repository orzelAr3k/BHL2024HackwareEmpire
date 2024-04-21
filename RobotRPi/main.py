import time
import adafruit_dht
from api_posting import MeasPoster
import board

import RPi.GPIO as GPIO

MIC_PIN = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(MIC_PIN, GPIO.IN)

mp = MeasPoster(2)

dht_device = adafruit_dht.DHT11(board.D4)

counter_limit = 20

counter = 0
temperature_acc = 0
humidity_acc = 0
noise_acc = 0

while True:
    try:
        temperature_acc += dht_device.temperature

        humidity_acc += dht_device.humidity
    except RuntimeError as err:
        continue
    
    if (GPIO.input(MIC_PIN) == True):
        noise_acc += 1

    counter+=1

    if(counter >= counter_limit):
        mp.publish(temperature_acc/counter, humidity_acc/counter, noise_acc > 3)
        print(f"temp: {temperature_acc/counter} deg, hum: {humidity_acc/counter}%, Loud: {noise_acc > 10} ({noise_acc} out of {counter})")
        temperature_acc = 0
        humidity_acc = 0
        noise_acc = 0
        counter = 0

    time.sleep(0.1)
    