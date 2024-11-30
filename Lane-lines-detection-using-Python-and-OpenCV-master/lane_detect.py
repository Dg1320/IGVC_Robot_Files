import matplotlib.pyplot as plt
from PIL import Image
import numpy as np
import os
import cv2  # Import OpenCV

# Define the function to display images
def list_images(images, cols=2, rows=None, cmap=None, output_file='output_images.png'):
    num_images = len(images)
    if rows is None:
        rows = (num_images + cols - 1) // cols  # Ceiling division

    plt.figure(figsize=(10, 11))
    
    for i, image in enumerate(images):
        plt.subplot(rows, cols, i + 1)
        cmap_to_use = 'gray' if len(image.shape) == 2 else cmap
        plt.imshow(image, cmap=cmap_to_use)
        plt.xticks([])
        plt.yticks([])

    plt.tight_layout(pad=0, h_pad=0, w_pad=0)
    
    # Save the figure to a file
    plt.savefig(output_file)  # Specify the output filename
    plt.close()  # Close the figure to free memory

# Load images from a specified folder
def load_images_from_folder(folder):
    images = []
    for filename in os.listdir(folder):
        img_path = os.path.join(folder, filename)
        try:
            img = Image.open(img_path)
            images.append(np.array(img))  # Convert PIL image to numpy array
        except Exception as e:
            print(f"Error loading {filename}: {e}")
    return images

def convert_hsl(image):
    """Convert an image from RGB to HSL."""
    return cv2.cvtColor(image, cv2.COLOR_RGB2HLS)

def HSL_color_selection(image):
    """Apply color selection to the HSL images to blackout everything except for white and yellow lane lines."""
    converted_image = convert_hsl(image)
    
    # White color mask
    lower_threshold = np.uint8([0, 200, 0])  # H, L, S
    upper_threshold = np.uint8([255, 255, 255])
    white_mask = cv2.inRange(converted_image, lower_threshold, upper_threshold)
    
    # Yellow color mask
    lower_threshold = np.uint8([10, 0, 100])  # H, L, S
    upper_threshold = np.uint8([40, 255, 255])
    yellow_mask = cv2.inRange(converted_image, lower_threshold, upper_threshold)
    
    # Combine masks
    mask = cv2.bitwise_or(white_mask, yellow_mask)
    masked_image = cv2.bitwise_and(image, image, mask=mask)
    
    return masked_image

def region_selection(image):
    """Determine and cut the region of interest in the input image."""
    mask = np.zeros_like(image)   
    if len(image.shape) > 2:
        channel_count = image.shape[2]
        ignore_mask_color = (255,) * channel_count
    else:
        ignore_mask_color = 255
    
    rows, cols = image.shape[:2]
    bottom_left  = [cols * 0.1, rows * 0.95]
    top_left     = [cols * 0.4, rows * 0.6]
    bottom_right = [cols * 0.9, rows * 0.95]
    top_right    = [cols * 0.6, rows * 0.6]
    vertices = np.array([[bottom_left, top_left, top_right, bottom_right]], dtype=np.int32)
    
    cv2.fillPoly(mask, vertices, ignore_mask_color)
    masked_image = cv2.bitwise_and(image, mask)
    
    return masked_image

def hough_transform(image):
    """Determine lines in the image using the Hough Transform."""
    rho = 1              # Distance resolution of the accumulator in pixels.
    theta = np.pi / 180  # Angle resolution of the accumulator in radians.
    threshold = 20       # Only lines that are greater than threshold will be returned.
    minLineLength = 20   # Line segments shorter than that are rejected.
    maxLineGap = 300     # Maximum allowed gap between points on the same line to link them.
    return cv2.HoughLinesP(image, rho=rho, theta=theta, threshold=threshold,
                           minLineLength=minLineLength, maxLineGap=maxLineGap)

def average_slope_intercept(lines):
    """Find the slope and intercept of the left and right lanes of each image."""
    left_lines = []  # (slope, intercept)
    left_weights = []  # (length,)
    right_lines = []  # (slope, intercept)
    right_weights = []  # (length,)
    
    for line in lines:
        for x1, y1, x2, y2 in line:
            if x1 == x2:
                continue
            slope = (y2 - y1) / (x2 - x1)
            intercept = y1 - (slope * x1)
            length = np.sqrt(((y2 - y1) ** 2) + ((x2 - x1) ** 2))
            if slope < 0:
                left_lines.append((slope, intercept))
                left_weights.append(length)
            else:
                right_lines.append((slope, intercept))
                right_weights.append(length)
    left_lane = np.dot(left_weights, left_lines) / np.sum(left_weights) if len(left_weights) > 0 else None
    right_lane = np.dot(right_weights, right_lines) / np.sum(right_weights) if len(right_weights) > 0 else None
    return left_lane, right_lane

def pixel_points(y1, y2, line):
    """Converts the slope and intercept of each line into pixel points."""
    if line is None:
        return None
    slope, intercept = line
    x1 = int((y1 - intercept) / slope)
    x2 = int((y2 - intercept) / slope)
    y1 = int(y1)
    y2 = int(y2)
    return ((x1, y1), (x2, y2))

def lane_lines(image, lines):
    """Create full length lines from pixel points."""
    left_lane, right_lane = average_slope_intercept(lines)
    y1 = image.shape[0]
    y2 = y1 * 0.6
    left_line = pixel_points(y1, y2, left_lane)
    right_line = pixel_points(y1, y2, right_lane)
    return left_line, right_line

def draw_lane_lines(image, lines, color=[255, 0, 0], thickness=12):
    """Draw lines onto the input image."""
    line_image = np.zeros_like(image)
    for line in lines:
        if line is not None:
            cv2.line(line_image, *line, color, thickness)
    return cv2.addWeighted(image, 1.0, line_image, 1.0, 0.0)

# Specify the folder containing your images
folder_path = '/home/clutch13/Desktop/test_images/'  # Adjust this if needed

# Load images from the specified folder
images = load_images_from_folder(folder_path)

# Apply HSL color selection to each image
hsl_masked_images = [HSL_color_selection(image) for image in images]

# Apply region selection to the HSL masked images
roi_masked_images = [region_selection(image) for image in hsl_masked_images]

# Apply Canny edge detection
canny_images = [cv2.Canny(image, 50, 150) for image in roi_masked_images]

# Apply Hough Transform to the Canny images
hough_lines = [hough_transform(image) for image in canny_images]

# Get lane lines for each image
lane_lines_list = [lane_lines(original, lines) for original, lines in zip(images, hough_lines)]

# Draw the detected lane lines on the original images
final_images = [draw_lane_lines(original, lines) for original, lines in zip(images, lane_lines_list)]

# Display the final images and save to file
list_images(final_images, cols=3, output_file='output_images.png')  # Adjust filename as needed
