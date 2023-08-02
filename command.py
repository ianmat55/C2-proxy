import socket, asyncio
import struct
from database.db import init_db
from enum import Enum

init_db()

'''
server should have features to manage and control agents:
    - registering new agents
    - agent authentication
    - track status
    - send and distribute tasks

ping all active agents on startup

should have task scheduling

store task history, results, logs in a db

authentication and security
    - only authorized agents can connect to server
    - encrypted communications

enable logging and monitoring to track activities of agents
    - alerts for critical events

some type of web interface
'''

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 3000  # Port to listen on (non-privileged ports are > 1023)

class MessageType(Enum):
    NetworkData = 0
    TaskResults = 1
    RegisterAgent = 2
    # Add more message types here as needed

def parseMessage(message_type, data):
    if message_type == MessageType.NetworkData.value:
        # Handle network data message
        print("Received Network Data:", data)
    elif message_type == MessageType.TaskResults.value:
        # Handle task results message
        print("Received Task Results:", data)
    elif message_type == MessageType.RegisterAgent.value:
        # Handle agent registration message
        print("Received Agent Registration:", data)
    # Add more cases for other message types as needed

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        print("listening for connections")
        s.listen()
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
            while True:
                message_data = conn.recv(1024)
                if not message_data:
                    break
                
                # Assuming the first 4 bytes represent the MessageHeader
                header_data = message_data[:8]
                header = struct.unpack('II', header_data)  # Assuming 'II' is the correct format for MessageHeader
                
                # Extract the body part of the message (excluding the header)
                body_data = message_data[4:]

                print(f"Received Message Type: {header[0]}")
                print(f"Received Body: {body_data}")
                
        s.close()

if __name__ == "__main__":
    main()