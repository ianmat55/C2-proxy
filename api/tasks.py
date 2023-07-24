import uuid
import json
from database.models import Task

from flask import request, Response
from flask_restful import Resource

class TasksResource(Resource):
    def get(self):
        tasks = Task.objects().to_json()
        return Response(tasks, mimetype="application/json", status=200)
    
    # send command to agents, could be simple ping, web scraping files, etc
    def post(self):
        body = request.get_json()
        json_obj = json.loads(json.dumps(body))
        obj_num = len(body)

        for i in range(len(body)):
            json_obj[i]['task_id'] = str(uuid.uuid4())
            Task(**json_obj[i].save())
            task_options = []
            
            for key in json_obj[i].keys():
                if (key != "task_type" and key != "task_id"):
                    task_options.append(key + ": " + json_obj[i][key])

        return Response(Task.objects.skip(Task.objects.count() - obj_num).to_json(), mimetype="application/json", status=200)
