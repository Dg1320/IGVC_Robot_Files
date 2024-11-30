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
    """
    Determine lines in the image using the Hough Transform.
        Parameters:
            image: The output of a Canny transform.
    """
    rho = 1              # Distance resolution of the accumulator in pixels.
    theta = np.pi / 180  # Angle resolution of the accumulator in radians.
    threshold = 20       # Only lines that are greater than threshold will be returned.
    minLineLength = 20   # Line segments shorter than that are rejected.
    maxLineGap = 300     # Maximum allowed gap between points on the same line to link them.
    return cv2.HoughLinesP(image, rho=rho, theta=theta, threshold=threshold,
                           minLineLength=minLineLength, maxLineGap=maxLineGap)

def draw_lines(image, lines):
    """Draw the lines on the image."""
    line_image = np.zeros_like(image)
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            cv2.line(line_image, (x1, y1), (x2, y2), (255, 0, 0), 2)  # Draw in blue color
    return cv2.addWeighted(image, 0.8, line_image, 1, 0)

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

# Draw the detected lines on the original images
final_images = [draw_lines(original, lines) for original, lines in zip(images, hough_lines)]

# Display the final images and save to file
list_images(final_images, cols=3, output_file='output_images.png')  # Adjust filename as needed
