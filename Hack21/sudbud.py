import matplotlib.pyplot as plt
import numpy as np
import time
import random
from time import sleep
import urllib.request

#IP address for communication
url = "http://192.168.1.48"

# Simple function to assist in communication with Arduino
def get_data():
	global data

	n = urllib.request.urlopen(url).read() # get the raw html data in bytes (sends request and warn our esp8266)
	n = n.decode("utf-8") # convert raw html bytes format to string

	data = n

# Generates a list of 30 random numbers between 75 and 100 to model previous 30 days
day = []
day_c = []
for i in range(0,30):
    n = random.randint(90,100)
    day_c.append(n)
    day.append(i)


while True:

    x = []
    y = []

    hour = 1 #start loop
    

    while hour <= 24: #set to 24 for 24 hours / day

        data_arr = []

        # Loop for __ amount of time and gather data from Arduino
        t_end = time.time() + 10 #* 60 for minutes conversion
        while time.time() < t_end:
            # Add arduino data to array (0 incomplete, 1 complete?)
            get_data()
            print(data)

            if (data == 1):
                data_arr.append(0)
            if (data == 2):
                data.append(1)

            print(data_arr)

        # Calculate % compliance
        size = len(data_arr)
        num_c = sum(data_arr)
        per_c = num_c/size * 100

        # Compliance (for last hour; pie chart)
        plt.figure(1)
        current_c = [per_c, 100 - per_c]
        l = ['Complete Cycles','Incomplete Cycles']
        plt.pie(current_c, labels = l, colors = ['#00bf26','#ff0d00'], autopct='%1.1f%%')
        plt.title('Handwashing Compliance\n(past hour)')

        plt.savefig('fig1.png')
        plt.clf()

        # Tracking daily compliance (by hour; line plot)
        x.append(hour)
        y.append(per_c)

        plt.figure(2)
        plt.plot(x, y, 'g-o')
        plt.title('Handwashing Compliance\n(24 hours)')
        plt.ylabel('Compliance(%)')
        plt.xlabel('Time (by hour)')
        plt.xticks(np.linspace(0,24,13))
        plt.yticks(np.linspace(0,100,11))

        plt.savefig('fig2.png')
        plt.clf()

        hour += 1
    
    day_c.pop(0)
    day_c.append(np.average(y))

    # Trackig compliance over last 30 days (line plot)
    plt.figure(3)
    plt.plot(day, day_c, 'g-')
    plt.title('Handwashing Compliance\n(30 days)')
    plt.ylabel('Compliance(%)')
    plt.xlabel('Day')
    plt.xticks(np.linspace(30,0,16))
    plt.yticks(np.linspace(0,100,11))

    plt.savefig('fig3.png')
    plt.clf()


