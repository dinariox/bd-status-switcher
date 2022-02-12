# === IMPORTS =================================================================
import sys
import glob
import serial
import serial.tools.list_ports
import time
import requests
import threading
# === END IMPORTS =============================================================

# === GLOBAL VARIABLES ========================================================
# BAUDRATE = 38400
BAUDRATE = 500000
SERIAL_TIMEOUT = 1
DISCORD_STATUS_UPDATE_INTERVAL = 0.5
selected_port = None
# === END GLOBAL VARIABLES ====================================================

# === FUNCTIONS ===============================================================
def write_read(arduino, x):
    arduino.write(x.encode())
    time.sleep(.01)
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
            name = write_read(s, 'name') # try twice because the Arduino sends garbage on the first read
            s.close()
            result.append([port, name])
        except (OSError, serial.SerialException):
            pass
    return result

def acceptArduinoUpdates():
    while True:
        try:
            data = arduino.readline()
            if (data):
                    data = data.decode('utf-8').rstrip()
                    
                    if (data.startswith('status')):
                        status = data.split(" ")[1]
                        print(f'Arduino requested: switch status to {status}')
                        requests.post('http://localhost:5142/', json={'status': status}, timeout=3)
                    elif (data.startswith('microphoneMuted')):
                        muted = data.split(" ")[1]
                        print(f'Arduino requested: switch microphone muted to {muted}')
                        requests.post('http://localhost:5142/', json={'microphoneMuted': muted}, timeout=3)
                    elif (data.startswith('headphoneMuted')):
                        muted = data.split(" ")[1]
                        print(f'Arduino requested: switch headphone muted to {muted}')
                        requests.post('http://localhost:5142/', json={'headphoneMuted': muted}, timeout=3)
        # except serial.serialutil.SerialException:
        #     print('Lost connection to Arduino')
        #     time.sleep(1)
        #     arduino = serial.Serial(port=selected_port, baudrate=BAUDRATE, timeout=SERIAL_TIMEOUT)
        #     pass
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
            resp = requests.get('http://localhost:5142/', timeout=3)
            if (resp.status_code == 200):
                status = resp.json()['status']
                microphoneMuted = resp.json()['microphoneMuted']
                headphoneMuted = resp.json()['headphoneMuted']
                print(f'Current discord status: {status}, microphone muted: {"true" if microphoneMuted else "false"}, headphone muted: {"true" if headphoneMuted else "false"}')
                arduino.write(f'setstatus {status} {"true" if microphoneMuted else "false"} {"true" if headphoneMuted else "false"}'.encode())
        except:
            print(f'Discord status not available')
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
    print('Searching for Arduino...')

    # Get all available serial ports and scan for Arduino
    selected_port = None
    while(selected_port == None):
        ports = serial.tools.list_ports.comports()
        for port, desc, hwid in sorted(ports):
            if ("CP210x" in desc): # ESP32 contains "CP210x" in the description
                selected_port = port
                print(f'Found Arduino on {selected_port}')
                break
        time.sleep(1)

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