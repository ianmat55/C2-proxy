import os
import certifi
# from pymongo.mongo_client import MongoClient
from mongoengine import connect
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()
mongo_uri = os.getenv('MONGO_URI')

def init_db():
    conn = connect("Cluster0", host=mongo_uri, tlsCAFile=certifi.where())
    
    if conn is not None and conn.server_info():
        print("Connected to the MongoDB database.")
    else:
        print("Failed to connect to the MongoDB database.")
    