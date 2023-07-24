import uuid
import json
from database.models import TaskHistory

from flask import request, Response
from flask_restful import Resource

class HistoryResource(Resource):
    def __init__(self):
        pass