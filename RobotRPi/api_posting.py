import requests

class MeasPoster:
    def __init__(self, station_id:int) -> None:
        self.headers =  {"Content-Type":"application/json"}
        self.api_url = "http://192.168.241.195:3000/measurements"
        self.station_id = station_id
    def publish(self, temp:int, humid:int, noise:bool):
        todo = {"measurements": { "temperature": temp, "noise": noise, "humidity": humid }, "stationId": self.station_id}
        response = requests.post(self.api_url, json=todo, headers=self.headers)