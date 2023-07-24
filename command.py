import json
import os
import certifi

from flask import Flask
from flask_restful import Api

from api.results import ResultsResource
from api.tasks import TasksResource
from api.agents import AgentsResource

from database.db import init_db

app = Flask(__name__)
init_db()
api = Api(app)


# test = certifi.where()
# print(test)
# "/Users/ianmatsumoto/Desktop/c2/env/lib/python3.10/site-packages/certifi/cacert.pem"


'''
server should have features to manage and control agents:
    - registering new agents
    - agent authentication
    - track status
    - send and distribute tasks

should have task scheduling

store task history, results, logs in a db

authentication and security
    - only authorized agents can connect to server
    - encrypted communications

enable logging and monitoring to track activities of agents
    - alerts for critical events

some type of web interface
'''

api.add_resource(AgentsResource, "/agents", "/agents/<string:agent_id>")
api.add_resource(TasksResource, "/tasks", "/tasks/<int:task_id>")
api.add_resource(ResultsResource, "/results", "/results/<int:agent_id>")

@app.route("/")
def index():
    return "<p>Hello</p>"

if __name__ == '__main__':
    app.run(debug=True)
