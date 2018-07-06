# NodeMCU Greenhouse
An attempt at automating the watering of our new greenhouse using NodeMCU and various sensors.

The idea is to push sensor data to a backend (in this case my LAMP host, but should be Azure or AWS or other cloud service, had I the money) and query logic on the back end if there's some watering needed.

Possibly a fallback to let the mcu trigger some watering by itself if the backend is down.
