import datetime
from mongoengine import *

class Task(DynamicDocument):
    task_id = StringField(required=True)
    date_modified = DateTimeField(default=datetime.datetime.utcnow)

class TaskHistory(DynamicDocument):
    history_id = StringField(required=True)
    task_id = StringField(required=True)
    result_id = StringField(required=True)

class Result(DynamicDocument):
    result_id = StringField(required=True)
    date_modified = DateTimeField(default=datetime.datetime.utcnow)

class Agent(DynamicDocument):
    agent_id = StringField(required=True)
    name = StringField(required=True)
    ip = StringField(required=True)
    port = StringField(required=True)
    active = BooleanField(required=True)
    date_modified = DateTimeField(default=datetime.datetime.utcnow)

