"""
Take keyboard input and generate cmd_vel messages
"""

from pynput import keyboard
from collections import defaultdict
import time

keys = defaultdict(int)

def on_press(key):
    try:
        #print('alphanumeric key {0} pressed'.format(key.char))
        keys[key.char] = 1
    except AttributeError:
        print('special key {0} pressed'.format(key))

def on_release(key):
    # print('alphanumeric key {0} released'.format(key.char))
    keys[key.char] = 0
    if key == keyboard.Key.esc:
        print('stopping')
        exit(0)

with keyboard.Listener(on_press=on_press,
                        on_release=on_release) as listener:
    #listener.start()
    while True:
        print(keys)
        time.sleep(0.1)
