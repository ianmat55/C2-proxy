import json
import uuid
from database.models import Agent
from utils.message_types import MessageType
from agents import upsert_agent, remove_agent, get_agent, get_allAgents

# Define a function to handle agent responses
async def handle_agent_response(response_data, host, port, writer):
    try:
        # Parse the JSON response data
        response = json.loads(response_data.decode('utf-8'))

        # Extract relevant information from the response
        message_type = response["header"]["type"]
        agent_id = response["header"]["id"]
        message_data = response["body"]
        delim = "\r"

        request = ""

        if message_type == MessageType.NetworkData.value:
            # Handle network data message
            print("Received Network Data:", message_data)
        elif message_type == MessageType.TaskResults.value:
            # Handle task results message
            print("Received Task Results:", message_data)
        elif message_type == MessageType.RegisterAgent.value:
            # Handle agent registration message
            request = handle_registration(agent_id, host, port, message_data, writer)
        elif message_type == MessageType.Ping.value:
            # Handle ping
            request = handle_heartbeat(agent_id, message_data)
        
        request += delim

        writer.write(request.encode())
        await writer.drain()

    except json.JSONDecodeError as e:
        # Handle JSON parsing error
        print("JSON decoding error:", e)
    except Exception as e:
        # Handle other exceptions if needed
        print("Error:", e)

def handle_task_results(task_id, data):
    # Handle processing of task results
    # Example: Update task status, save results, etc.
    pass

def handle_heartbeat(agent_id, data):
    # Handle processing of heartbeat from the agent
    # Example: Update agent status, check agent health, etc.
    response = {
        'header' : {
            'agent_id' : agent_id,
            'type' : str(MessageType.Ping.value)
        },
        'body' : {
            'message' : 'pong'
        }
    }
    return json.dumps(response)

# Check if agent is registered based on id
# register agent in db if not registred.
def handle_registration(agent_id, host, port, data, writer):
    response = {
        'header' : {
            'agent_id' : agent_id,
            'type' : str(MessageType.RegisterAgent.value)
        }, 
    }

    agent = Agent.objects(agent_id=agent_id).first()
    
    if agent is None:
        try:
            new_agent_data = {
                'name': data['hostname'],
                'ip': host,
                'port': str(port),
                'active': True,
            }

            new_agent = Agent(**new_agent_data)
            #new_agent.save()
            response['header']['agent_id'] = str(uuid.uuid4())
            response["body"] = new_agent_data
            print("Agent successfully registered")
        except Exception as e:
            print("Error saving agent data:", e)
    else:
        response["body"] = agent

    upsert_agent(response['header']['agent_id'], writer)

    return json.dumps(response)
