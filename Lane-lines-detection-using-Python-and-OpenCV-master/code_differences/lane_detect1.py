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
    """
    Convert an image from RGB to HSL.
    """
    # Convert from RGB (0-255) to HLS (0-255)
    return cv2.cvtColor(image, cv2.COLOR_RGB2HLS)

def HSL_color_selection(image):
    """
    Apply color selection to the HSL images to blackout everything except for white and yellow lane lines.
        Parameters:
            image: An np.array compatible with plt.imshow.
    """
    # Convert the input image to HSL
    converted_image = convert_hsl(image)
    
    # White color mask
    lower_threshold = np.uint8([0, 200, 0])  # H, L, S
    upper_threshold = np.uint8([255, 255, 255])
    white_mask = cv2.inRange(converted_image, lower_threshold, upper_threshold)
    
    # Yellow color mask
    lower_threshold = np.uint8([10, 0, 100])  # H, L, S
    upper_threshold = np.uint8([40, 255, 255])
    yellow_mask = cv2.inRange(converted_image, lower_threshold, upper_threshold)
    
    # Combine white and yellow masks
    mask = cv2.bitwise_or(white_mask, yellow_mask)
    masked_image = cv2.bitwise_and(image, image, mask=mask)
    
    return masked_image

# Specify the folder containing your images
folder_path = '/home/clutch13/Desktop/test_images/'  # Adjust this if needed

# Load images from the specified folder
images = load_images_from_folder(folder_path)

# Apply HSL color selection to each image
masked_images = [HSL_color_selection(image) for image in images]

# Display the masked images and save to file
list_images(masked_images, cols=3, output_file='output_images.png')  # Adjust filename as needed
