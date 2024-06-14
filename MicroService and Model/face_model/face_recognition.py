from keras.models import load_model
import cv2
import numpy as np

class_to_name_dict={0:"Anastasia",2:"SomeOne"}

def find_face(img):
	face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
	try:
		gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
	except:
		return []
	faces = face_cascade.detectMultiScale(gray,1.3,5)
	res = []
	for x,y,w,h in faces:
		res.append(img[y:y+h,x:x+w])
	return res


def get_name_from_photo(source_image):
	img = find_face(source_image)
	if len(img) == 0:
		return "No people founded"
	else:
		img = img[0]
		classifier = load_model('face_model/face_recognition.h5')
		img = cv2.resize(img, (224, 224), interpolation = cv2.INTER_LINEAR)
		img = img / 255.
		img = img.reshape(1,224,224,3)
		res = np.argmax(classifier.predict(img, 1, verbose = 0))
		if(res == 0):
			return class_to_name_dict[res]
		else:
			return "Someone else"


if __name__ == "__main__":
	src = cv2.imread("face2.jpeg")
	print(get_name_from_photo(src))