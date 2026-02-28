import http.server
import cv2
import tensorflow as tf
import numpy as np
import socket
from cvlib.face_detection import detect_face


my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
model = tf.keras.models.load_model("model_Hope.h5")


esp32_IP = "192.168.4.1"
esp32_port = 80
esp8266_IP = "192.168.4.3"
esp8266_port = 80

class myhandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        
        if self.path == "/capture_images":
            
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            
            print("capture picture")

            image = self.get_images_from_esp32cam()

            print("image captured shape: ", image.shape)
            image = cv2.resize(image, (224,224))
            
            bbox, conf = detect_face(image)

            if bbox:
                selected_set = np.argmax(conf)
                image = image[bbox[selected_set][1]:bbox[selected_set][3], bbox[selected_set][0]:bbox[selected_set][2]]
                image = cv2.resize(image, (224,224))
                image = image.astype("float32") / 255
                image = np.expand_dims(image, axis=0)
            else:
                self.send_results_to_esp8266("2")
                return

            
            result = model.predict(image)
            print(result)
            result = round(float(result))
            
            if result == 1:
                print("Face recognized, result came out as : ",result)
            elif result == 0:
                print("Face NOT recognized, result came out as : ",result)

            self.send_results_to_esp8266(result)
            
            

        else:
            self.send_response(404)
            self.end_headers()

        return

    def get_images_from_esp32cam(self):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as esp32cam_socket:
                esp32cam_socket.connect((esp32_IP, esp32_port))

                esp32cam_socket.sendall(b"GET /capture HTTP/1.1\r\nHost:192.168.4.1\r\n\r\n")

                response = b""

                while b"\r\n\r\n" not in response:
                    response += esp32cam_socket.recv(1)

                header_data = response.split(b"\r\n\r\n")[0].decode("utf-8")
                lines = header_data.split("\r\n")

                for line in lines:
                    if line.startswith("Content-Length:"):
                        content_length = int(line.split(":")[1].strip())


                image_data = b""
                while len(image_data) < content_length:
                    image_data += esp32cam_socket.recv(1024)

                nparr = np.frombuffer(image_data, np.uint8)
                img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)


                cv2.waitKey(0)

                return img


                

        except Exception as e:
            print(f"failed error code : {e}")



    def send_results_to_esp8266(self, results):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as esp8266_socket:
                esp8266_socket.connect((esp8266_IP, esp8266_port))

                send_data = f"GET /image_process_result?result={results} HTTP/1.1\r\nHost:192.168.4.3\r\nConnection:close\r\n\r\n"
                esp8266_socket.sendall(send_data.encode())
                print("data sent to esp8266")

        except Exception as e:
            print(f"Failed due to reason : {e}")





                

            
                
                    

            

def run(s_class=http.server.HTTPServer, h_class=myhandler, port=8080):
    server_address = ('', port)
    httpd = s_class(server_address, h_class)
    httpd.serve_forever()


run()
