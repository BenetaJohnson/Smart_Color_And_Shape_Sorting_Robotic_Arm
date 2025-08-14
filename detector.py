import cv2
import numpy as np
import serial
import time

# Serial setup (adjust /dev/ttyAMA0 or /dev/serial0 based on your Pi's serial port)
ser = serial.Serial('/dev/serial0', 9600, timeout=1)
time.sleep(2)  # Wait for serial connection to initialize

# Define HSV color ranges for color detection
color_ranges = {
    "Red": [([0, 70, 50], [10, 255, 255]), ([170, 70, 50], [180, 255, 255])],  # two ranges for red wrap-around
    "Green": [([36, 50, 70], [89, 255, 255])],
    "Blue": [([90, 50, 70], [130, 255, 255])]
}


def detect_color(hsv_roi):
    for color, ranges in color_ranges.items():
        mask = None
        for lower, upper in ranges:
            lower = np.array(lower)
            upper = np.array(upper)
            curr_mask = cv2.inRange(hsv_roi, lower, upper)
            mask = curr_mask if mask is None else cv2.bitwise_or(mask, curr_mask)
        if cv2.countNonZero(mask) > 500:  # Threshold to confirm color presence
            return color
    return None


def detect_shape(contour):
    shape = "Unknown"
    peri = cv2.arcLength(contour, True)
    approx = cv2.approxPolyDP(contour, 0.04 * peri, True)
    vertices = len(approx)

    if vertices == 3:
        shape = "Triangle"
    elif vertices == 4:
        # Check aspect ratio for square or rectangle
        (x, y, w, h) = cv2.boundingRect(approx)
        ar = w / float(h)
        shape = "Square" if 0.95 <= ar <= 1.05 else "Rectangle"
    elif vertices > 7:
        shape = "Circle"
    else:
        shape = "Unknown"
    return shape


def main():
    cap = cv2.VideoCapture(0)  # Camera 0, your Pi camera
    time.sleep(2)  # Let camera warm up

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to grab frame")
            break

        # Blur and convert to HSV
        blurred = cv2.GaussianBlur(frame, (7, 7), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

        # Convert to grayscale for contour detection
        gray = cv2.cvtColor(blurred, cv2.COLOR_BGR2GRAY)
        _, thresh = cv2.threshold(gray, 60, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

        contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if contours:
            # Find largest contour (assumed object)
            c = max(contours, key=cv2.contourArea)
            if cv2.contourArea(c) > 1000:
                shape = detect_shape(c)
                mask = np.zeros(hsv.shape[:2], dtype="uint8")
                cv2.drawContours(mask, [c], -1, 255, -1)

                # Extract ROI for color detection
                hsv_roi = cv2.bitwise_and(hsv, hsv, mask=mask)
                color = detect_color(hsv_roi)

                if color and shape != "Unknown":
                    message = f"<{color},{shape}>"
                    print(f"Detected: {message}")
                    ser.write(message.encode())
                    time.sleep(3)  # Wait to avoid flooding serial

        # Optional: show frame for debugging (comment out if headless)
        # cv2.imshow("Frame", frame)
        # if cv2.waitKey(1) & 0xFF == ord('q'):
        #     break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
