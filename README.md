# Face-recognition-system-using-Tensorflow
A Face recognition system built using ESP8266, ESP32 and a Python Tensorflow pipeline

## Basic architecture
The system works by creating a server at every endpoint, the ESP8266, ESP32 Cam, and the computer. Each device acts as a client and a server ensuring communication between all 3 devices.
The model itself trained in a colabs environment is run on the laptop due to the memory constraints on the ESP modules.

### The ESP32 Cam
This is the device that captures the picture of the person and then feeds it to the server on the computer to run inference on it.
The ESP32 Cam is configured with the esp_camera.h library. This acts as a server only and the laptop connects to this as a client and requests the image. The code for the esp32cam can be seen in the "Face recognition project.ino" file.

### The esp8266
This is the device responisble for handling the button that snaps the picture and th LCD screen that displays the information. I was using an I2C driver to control the LCD display. The ESP8266 acts as both a server and a client. It connects to the laptop's server and the laptop also connects to the esp8266's server. This module checks for the button press and commands the laptop to request an image. After processing the image, the laptop connects to the esp8266's server and sends the results to it to display.

### The laptop
The laptop in this case manages the model and everything related to processing the image. It acts as both a server and a client. The laptop maintains connections with both the other modules hence acts as the mediator. Once the button press is recognized by the ESP8266, the laptop requests an image from the ESP32Cam and then once received, it will process the image and run inference on it. Once the results are obtained, they are sent to the esp8266 to display.


# The model and the dataset
For this purpose I have utilized google colabs as I do not posess the computational power to train such a model. The model is created using TensorFlow and is a Convultional Neural Network. As the dataset I have utilized an open source dataset from kaggle, https://www.kaggle.com/datasets/ashwingupta3012/male-and-female-faces-dataset

## Preparing the dataset
The dataset itself contains about 5500 faces of random people which in itself is no use to me. So I created 3, 20 second video clips of my face in various lighting conditions and extracted the frames of them. This gave me about 2000 images of myself which then I copied to make 6000 total images of myself. Then after joining the two datasets, I used a random multiplier to simulate even more diverse lighting conditions. After this I ran the function detect_face on each image and extracted only the best images out of them. This provided me with good quality images.

## The model
Here I used a CNN model with 3 Conv 2D layers and 2 dense layers at the end with a sigmoid function to obtain the strength of the signal. 


<img width="793" height="270" alt="image" src="https://github.com/user-attachments/assets/48bad1ff-38a0-447e-a400-63e3eb571ec9" />

<img width="368" height="306" alt="image" src="https://github.com/user-attachments/assets/27dd59bf-ec43-4f1e-9aec-aeeb007e5af9" />



## Traing the model
Here I used 'Adam' optimizer and 'binary_crossentropy' loss function to obtain exceptional performance. After compiling the model I trained the model for 100 epochs with batch size of 64. After alot of messing around with the parameters and re-training hundreds of times I finally got considerablly good performance.

<img width="433" height="321" alt="image" src="https://github.com/user-attachments/assets/c335cee3-7369-4f4b-aac3-a3aff93bedf7" />


<img width="427" height="322" alt="image" src="https://github.com/user-attachments/assets/e42cc855-b77e-454c-a485-d513424dde08" />



