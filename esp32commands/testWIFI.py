import webbrowser
import time
import os

# urls to send to firefox
forward_url = "http://192.168.4.1/FORWARD"
backward_url = "http://192.168.4.1/BACKWARD"
right_url = "http://192.168.4.1/RIGHT"
left_url = "http://192.168.4.1/LEFT"

webbrowser.open(forward_url)

time.sleep(1)                         

webbrowser.open(backward_url)

time.sleep(1)  

webbrowser.open(right_url)

time.sleep(1)  

webbrowser.open(left_url)
time.sleep(1)                          # wait 1 seconds

os.system("pkill firefox")              # close firefox so we dont waste memory
                                        # would close every time , but when doing so causes some error with firefox sometimes
                                        # so instead we can open tabs to a certain tab limit, then close the window afterwards
                                        # this way is faster as well
                                    

