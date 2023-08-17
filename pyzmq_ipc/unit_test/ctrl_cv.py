import keyboard
import clipboard

from pyzmq_ipc import ZmqIpc

import time
from typing import Callable


class ClipboardListener:
    def __init__(self):
        self.previous_clipboard = self.get_clipboard()
        self.on_copied: Callable = lambda: print("Copied: Ctrl+C")
        self.on_pasted: Callable = lambda: print("Pasted: Ctrl+V")

    def _setup_listeners(self):
        keyboard.add_hotkey('ctrl+c', self.on_copied)
        keyboard.add_hotkey('ctrl+v', self.on_pasted)

    def start(self):
        print("Listening for copy and paste operations.")
        self._setup_listeners()

    @classmethod
    def get_clipboard(cls):
        return clipboard.paste()

    def check_clipboard_change(self) -> bool:
        current_clipboard = self.get_clipboard()
        if current_clipboard != self.previous_clipboard:
            self.previous_clipboard = current_clipboard
            return True
        else:
            return False


if __name__ == "__main__":
    zmq_ipc = ZmqIpc()
    on_copied = zmq_ipc.publish()
    on_pasted = zmq_ipc.publish()

    listener = ClipboardListener()
    listener.on_copied = on_copied
    listener.on_pasted = on_pasted
    listener.start()

    while True:
        print("---")
        time.sleep(1)
