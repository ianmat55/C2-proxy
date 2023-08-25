from enum import Enum

class MessageType(Enum):
    NetworkData = 0
    TaskResults = 1
    RegisterAgent = 2
    Ping = 3
    # Add more message types here as needed