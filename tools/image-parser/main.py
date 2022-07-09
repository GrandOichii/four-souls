import pandas as pd
import easyocr
import cv2

SRC = '../../game/images/001D6(4).bmp'

img = cv2.imread('happy.jpg')
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
noise=cv2.medianBlur(gray,3)
thresh = cv2.threshold(noise, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]

cv2.imshow('image',img)
cv2.waitKey(0)

reader = easyocr.Reader(['en'])
result = reader.readtext(img,paragraph="False")
df = pd.DataFrame(result)
print(df[1])