import requests
import time
import os
from datetime import datetime
import cv2
import numpy as np

# ESP32-CAM IP address - change this to match your camera's IP
ESP32_IP = "192.168.4.1"  # Default IP when ESP32 is in AP mode
URL = f"http://{ESP32_IP}/stream"

# Create images directory if it doesn't exist
if not os.path.exists("images"):
    os.makedirs("images")
    print("Created 'images' directory")

def capture_frames():
    print(f"Connecting to ESP32-CAM at {URL}...")
    
    try:
        # Open stream
        response = requests.get(URL, stream=True)
        if response.status_code != 200:
            print(f"Failed to connect to ESP32-CAM. Status code: {response.status_code}")
            return
            
        print("Connected to stream. Capturing frames...")
        
        # Variables for parsing MJPEG stream
        bytes_data = bytes()
        start_time = time.time()
        frame_count = 0
        
        # Capture for 5 seconds
        while time.time() - start_time < 5.0:
            # Read chunks from the stream
            for chunk in response.iter_content(chunk_size=1024):
                bytes_data += chunk
                
                # Find the JPEG frame boundaries
                a = bytes_data.find(b'\xff\xd8')  # JPEG start
                b = bytes_data.find(b'\xff\xd9')  # JPEG end
                
                if a != -1 and b != -1:
                    # Extract the JPEG image
                    jpg = bytes_data[a:b+2]
                    bytes_data = bytes_data[b+2:]
                    
                    # Convert bytes to image
                    img = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                    if img is None:
                        continue
                    
                    # Save the image
                    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")[:-3]
                    filename = f"images/frame_{timestamp}.jpg"
                    cv2.imwrite(filename, img)
                    
                    frame_count += 1
                    print(f"Saved frame {frame_count}: {filename}")
                    
                    # Wait for 400ms before capturing the next frame
                    time.sleep(0.2)
                    
                    # Check if 5 seconds have passed
                    if time.time() - start_time >= 20.0:
                        break
            
            # Check if 5 seconds have passed
            if time.time() - start_time >= 20.0:
                break
                
        print(f"Capture complete. Saved {frame_count} frames in 'images' directory.")
        
    except Exception as e:
        print(f"Error: {str(e)}")
    finally:
        # Close the connection
        if 'response' in locals():
            response.close()

if __name__ == "__main__":
    capture_frames()
