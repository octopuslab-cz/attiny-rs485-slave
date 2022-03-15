import time
from machine import Pin, UART

u = UART(2, 9600, rx=36, tx=4)
rts = Pin(5, Pin.OUT)

def send(data):
  rts(1)
  u.write(data)
  time.sleep_ms(10)
  rts(0)

def read():
  while True:
    if u.any():
        print(u.read())

        
send(b'\x05\x03\x10')
send(b'\x05\x06\x10\x20')
