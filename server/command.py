import asyncio
import socket
import json
from database.db import init_db
from task_handler import handle_agent_response

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

async def handle_agent_connection(reader, writer):
    addr = writer.get_extra_info('peername')
    host = addr[0]
    port = addr[1]

    while True:
        try:
            message_data = await reader.readuntil(b'\r')  # Read until a newline character
            message_data = message_data.strip()  # Convert to string and remove newline

            if not message_data:
                break

            await handle_agent_response(message_data, host, port, writer)

        except asyncio.CancelledError:
            print("BREAK")
            break

    print(f"Connection closed by {addr}")
    writer.close()
    await writer.wait_closed()

async def main():
    server = await asyncio.start_server(
        handle_agent_connection, HOST, PORT)

    print("Listening for connections")
    async with server:
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())