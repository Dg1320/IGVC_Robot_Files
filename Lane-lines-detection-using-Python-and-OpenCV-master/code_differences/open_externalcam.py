import cv2

# read webcam
webcam = cv2.VideoCapture("/dev/video2")         # i only have 1 webcam on my computer therefore 0

# visualize webcam

while True:
    ret, frame = webcam.read()

    cv2.imshow('frame',frame)
    if  cv2.waitKey(40) & 0xFF == ord('q'):                # wait 40ms and break when the letter q is pressed
        break

webcam.release()
cv2.destroyAllWindows()
