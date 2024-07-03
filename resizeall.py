import os
from PIL import Image, ExifTags

def resize_images_in_folder(input_folder, output_folder, size):
    # Create the output folder if it doesn't exist
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    
    # Get the list of files in the input folder
    files = os.listdir(input_folder)
    
    for file in files:
        # Get the file extension
        file_name, file_extension = os.path.splitext(file)
        
        # Check if the file is an image
        if file_extension.lower() in ['.jpg', '.jpeg', '.png', '.gif']:
            # Set the input and output file paths
            input_file_path = os.path.join(input_folder, file)
            output_file_path = os.path.join(output_folder, file)
            
            # Resize and fix the image orientation
            resize_image(input_file_path, output_file_path, size)

def resize_image(input_image_path, output_image_path, size):
    try:
        with Image.open(input_image_path) as image:
            # Fix image orientation if necessary
            fixed_image = fix_image_orientation(image)
            
            # Resize the image
            fixed_image.thumbnail(size, Image.ANTIALIAS)
            fixed_image.save(output_image_path, "JPEG")
            print(f"Image resized and saved: {output_image_path}")
    except IOError:
        print(f"Unable to resize image: {input_image_path}")

def fix_image_orientation(image):
    try:
        # Fix orientation based on EXIF metadata
        for orientation in ExifTags.TAGS.keys():
            if ExifTags.TAGS[orientation] == 'Orientation':
                break
        exif = image._getexif()
        if exif is not None:
            exif_data = dict(exif.items())
            if orientation in exif_data:
                if exif_data[orientation] == 3:
                    image = image.rotate(180, expand=True)
                elif exif_data[orientation] == 6:
                    image = image.rotate(270, expand=True)
                elif exif_data[orientation] == 8:
                    image = image.rotate(90, expand=True)
    except (AttributeError, KeyError, IndexError):
        pass
    
    return image

# Set the input folder containing the images
input_folder = "datasets/test/"

# Set the output folder for the resized images
output_folder = "datasets/resizedTest/"

# Set the target size for the resized images
target_size = (800, 800)

# Resize the images in the folder
resize_images_in_folder(input_folder, output_folder, target_size)