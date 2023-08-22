import os
import asyncio
from database.db import init_db
from response_handler import handle_agent_response
from aiohttp import web
from dotenv import load_dotenv
from agents import agent_writers, get_allAgents

load_dotenv()
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

incoming_queue = asyncio.Queue()
outgoing_queue = asyncio.Queue()

# API LIST
async def hello(request):
    return web.Response(text="Hello, world")

async def handle_proxy(request):
    return web.Response(text="Handle proxy")

def handle_get_agents(data):
    agent_ids = get_allAgents()
    print(agent_ids)
    response_data = {"agent_ids": agent_ids}
    return web.json_response(response_data) 

async def ping(request):
    print('request ping')
    for agent_id in agent_writers:
        writer = agent_writers[agent_id]
        ping = "ping"
        writer.write(ping.encode())
        await writer.drain()

    return web.Response(text='Ping')

async def handle_task(request):
    try:
        data = await request.json()  # Assuming the payload contains JSON data
        task = data.get('task')  # Extract the task from the JSON data
        if task:
            print(task)
            # await outgoing_queue.put(task)
            # return web.json_response({'status': 'Task enqueued'})
        else:
            return web.json_response({'error': 'Invalid request'}, status=400)
    except Exception as e:
        return web.json_response({'error': str(e)}, status=500)

async def start_http_server():
    try:
        app = web.Application()
        
        app.add_routes([web.get('/', hello),
                web.get('/agents', handle_get_agents),
                web.get('/fetch', handle_proxy),
                web.get('/ping', ping),
                web.post('/enqueue_task', handle_task),])

        runner = web.AppRunner(app)
        await runner.setup()
        site = web.TCPSite(runner, os.getenv('HOST'), os.getenv('HTTP_PORT'))
        await site.start()

        print("HTTP server started and listening...")

    except Exception as e:
        print("An error occurred while starting the HTTP server:")
        print(str(e))  # Print the error message for debugging

# SOCKET CONNECTION
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

            await incoming_queue.put(message_data)  # Enqueue the incoming message

            #TODO: replace message data with queued task
            asyncio.create_task(handle_agent_response(message_data, host, port, writer))

        except asyncio.CancelledError:
            break

    print(f"Connection closed by {addr}")
    writer.close()
    await writer.wait_closed()

async def main():
    socket_server = await asyncio.start_server(
        handle_agent_connection, os.getenv('HOST'), os.getenv('SOCK_PORT'))
    
     # Start the HTTP server concurrently
    http_server_task = asyncio.create_task(start_http_server())

    # Run both servers concurrently using context managers
    await asyncio.gather(socket_server.serve_forever(), http_server_task)

if __name__ == "__main__":
    asyncio.run(main())