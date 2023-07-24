from flask import Flask
from flask_restful import Resource

class ResultsResource(Resource):
    def get(self):
        pass
    # send command to agents, could be simple ping, web scraping files, etc
    def post(self):
        pass 
