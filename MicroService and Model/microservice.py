import json
from flask import Flask
from flask import url_for
from flask import jsonify
from flask import request
from flask import make_response
from flask import Flask, flash, request, redirect, url_for
from multiprocessing import Process
from PIL import Image
from sys import getsizeof as sizeof
import cv2
from face_model import face_recognition#.py

app = Flask(__name__)


@app.route('/picture', methods=['POST'])
def upload_data():
    img_data = request.get_data()
    with open("temp.jpg", "wb") as f:
       f.write(img_data)
    img_data = cv2.imread("temp.jpg")
    res = face_recognition.get_name_from_photo(img_data)
    return res


@app.route("/", methods=["GET","POST"])
def main():
    return f"Hello!"


app.run(host="0.0.0.0", debug=True)


