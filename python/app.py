# === IMPORTS =================================================================
import sys
import glob
import serial
import time
# === END IMPORTS =============================================================

# === GLOBAL VARIABLES ========================================================
BAUDRATE = 38400
SERIAL_TIMEOUT = 1
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

    # Accept commands forever
    while True:
        command = input('Enter a command: ')
        value = write_read(arduino, command)
        print('--> ' + value)
# === END MAIN ===============================================================