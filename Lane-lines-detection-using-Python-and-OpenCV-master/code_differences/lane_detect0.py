import matplotlib.pyplot as plt
from PIL import Image
import numpy as np
import os

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

# Specify the folder containing your images
folder_path = '/home/clutch13/Desktop/test_images/'  # Adjust this if needed

# Load images from the specified folder
images = load_images_from_folder(folder_path)

# Display the images and save to file
list_images(images, cols=3, output_file='output_images.png')  # Adjust filename as needed
