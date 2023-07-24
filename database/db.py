import os
import certifi
# from pymongo.mongo_client import MongoClient
from mongoengine import connect
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()
mongo_uri = os.getenv('MONGO_URI')

def init_db():
    connect("Cluster0", host=mongo_uri, tlsCAFile=certifi.where())