# === IMPORTS =================================================================
import sys
import glob
from turtle import delay
import serial
import time
import requests
import threading
# === END IMPORTS =============================================================

# === GLOBAL VARIABLES ========================================================
BAUDRATE = 38400
SERIAL_TIMEOUT = 1
DISCORD_STATUS_UPDATE_INTERVAL = 3
# === END GLOBAL VARIABLES ====================================================

# === FUNCTIONS ===============================================================
def write_read(arduino, x):
    arduino.write(x.encode())
    time.sleep(.1)
    data = arduino.readline()
    try:
        data = data.decode('utf-8')
    except:
        pass
    return data

def serial_ports():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port=port, baudrate=BAUDRATE, timeout=SERIAL_TIMEOUT)
            name = write_read(s, 'name')
            s.close()
            result.append([port, name])
        except (OSError, serial.SerialException):
            pass
    return result

def acceptArduinoUpdates():
    while True:
        data = arduino.readline()
        if (data):
            try:
                data = data.decode('utf-8').rstrip()
                
                if (data.startswith('status')):
                    status = data.split(" ")[1]
                    print(f'Arduino requested: switch status to {status}')
                    requests.post('http://localhost:5142/', json={'status': status}, timeout=3)
            except:
                pass

class acceptArduinoUpdatesThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        acceptArduinoUpdates()

def checkDiscordStatus():
    while True:
        try:
            resp = requests.get('http://localhost:5142/')
            if (resp.status_code == 200):
                print(f'Current discord status: {resp.text}')
                arduino.write(f'setstatus {resp.text}'.encode())
        except:
            pass
        time.sleep(DISCORD_STATUS_UPDATE_INTERVAL)
    
class checkDiscordStatusThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        checkDiscordStatus()
# === END FUNCTIONS ==========================================================

# === MAIN ===================================================================
if __name__ == '__main__':
    print('=== ARDUINO COMMUNICATION ===')
    print('Detecting serial ports...')

    # Detect available serial ports and print them
    ports = serial_ports()
    for (port, name) in ports:
        print(f'{port}: {name}')

    # Ask user to select a port
    selected_port = input('Select port: ')
    while (selected_port not in [port[0] for port in ports]):
        print('Invalid port!')
        selected_port = input('Select port: ')

    # Open selected port
    arduino = serial.Serial(port=selected_port, baudrate=BAUDRATE, timeout=SERIAL_TIMEOUT)

    thread1 = acceptArduinoUpdatesThread()
    thread1.daemon = True
    thread1.start()

    thread2 = checkDiscordStatusThread()
    thread2.daemon = True
    thread2.start()

    input()

    # Accept commands forever
    # while True:
    #     command = input('Enter a command: ')
    #     value = write_read(arduino, command)
    #     print('--> ' + value)
# === END MAIN ===============================================================