import uuid
import socket
import traceback
import json
from database.models import Agent
from mongoengine import *

from flask import request, make_response
from flask_restful import Resource

class AgentsResource(Resource):
    def get(self, agent_id=None):
        try:
            if agent_id:
                agent = Agent.objects(agent_id=agent_id).first()
                if not agent:
                    return {"message": "Agent not found."}, 404

                agent_data = {
                    "agent_id": agent.agent_id,
                    "name": agent.name,
                    "ip": agent.ip,
                    "port": agent.port
                }

                return agent_data, 200

            agents = Agent.objects.all()
            agent_data = [ {"agent_id": agent.agent_id, "name": agent.name, "ip": agent.ip, "port": agent.port} for agent in agents ]
            return agent_data, 200
        except Exception as e:
            return {"message": "Failed to get agent(s)", "error": str(e)}, 500 

    def post(self):
        try:
            agent_id = str(uuid.uuid4())  # Generate a unique agent ID
            remote_ip = request.remote_addr
            port = request.environ.get('SERVER_PORT')
            hostname = socket.gethostname()
            
            print("registering agent")
            # Create a new Agent document using the model
            agent = Agent(agent_id=agent_id, name=hostname, ip=remote_ip, port=port, active=True)
            agent.save()
            print("success")

            response_data = {
                "message": "Agent registered successfully",
                "agent_id": agent_id
            }
            
            response = make_response(json.dumps(response_data), 201)
            response.headers['Content-Type'] = 'application/json'
            return response

        except Exception as e:
            traceback.print_exc()
            return {"message": "Failed to register agent", "error": str(e)}, 500
