import time
import requests
import httplib as http

# For testing on normal PC: Mark as false to get rid of GPIO stuff and output 
# api file in  local dir.
RUNNING_ON_RPI = False  

# Get price from there following url. If the price is <50 the api of this
# server outputs "green", otherwise "red".
url = "https://tiipwilab.eng.au.dk/exercises/group5/price.txt"

if RUNNING_ON_RPI:
    import RPi.GPIO as GPIO
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(20, GPIO.OUT, pull_up_down=GPIO.PUD_DOWN)
    GPIO.setup(21, GPIO.OUT, pull_up_down=GPIO.PUD_DOWN)
    api_file = "/var/www/LED_Color.txt"
else:
    api_file = "./LED_Color.txt"

while True:
    r = requests.get(url)
    if r.status_code is not http.OK:
        print("Unable to get price from %s, HTTP status code: %i", url, r.status_code)
        if RUNNING_ON_RPI:
            GPIO.output(20,0)
            GPIO.output(21,0)
    else:
        price = int(r.text)
        cheap = price < 50
        
        print("price: %d -> cheap: %s" % (price, cheap))
        
        if not cheap:
            color = "red"
            if RUNNING_ON_RPI:
                GPIO.output(20,1)     
        else:
            color = "green"
            if RUNNING_ON_RPI:
                GPIO.output(21,1)    
            
        file = open(api_file, 'w+')
        file.truncate()
        file.write(color)
        file.close()

    time.sleep(1)

if RUNNING_ON_RPI:  
    GPIO.cleanup() 
