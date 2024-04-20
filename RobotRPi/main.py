import time
import adafruit_dht
import board

import RPi.GPIO as GPIO

MIC_PIN = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(MIC_PIN, GPIO.IN)

dht_device = adafruit_dht.DHT11(board.D4)

while True:
    try:
        temperature = dht_device.temperature

        humidity = dht_device.humidity

        print(f'Temp: {temperature} C  Humidity: {humidity} %')
    except RuntimeError as err:
        print(err.args[0])

    
    if (GPIO.input(MIC_PIN) == True):
        print('It is loud!')
    else:
        print('It is quiet enough!')

    time.sleep(0.5)
    