import threading

agent_writers = {}
lock = threading.Lock()

def upsert_agent(agent_id, agent_info):
    global agent_writers
    with lock:
        agent_writers[agent_id] = agent_info

def remove_agent(agent_id):
    global agent_writers
    with lock:
        if agent_id in agent_writers:
            del agent_writers[agent_id]

def get_allAgents():
    global agent_writers
    with lock:
        agent_ids = list(agent_writers.keys())
        return agent_ids

def get_agent(agent_id):
    global agent_writers
    with lock:
        return agent_writers.get(agent_id)