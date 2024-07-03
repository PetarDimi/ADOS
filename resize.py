from PIL import Image

def resize_image(image_path, output_path, size):
    try:
        with Image.open(image_path) as image:
            image.thumbnail(size, Image.ANTIALIAS)
            image.save(output_path, "JPEG")
            print("Image resized successfully!")
    except IOError:
        print("Unable to resize image.")

# Set the input image path and output path for the resized image
input_image_path = "slika1.jpg"
output_image_path = "slika1_resized_image.jpg"

# Set the target size for the resized image
target_size = (800, 800)

# Resize the image
resize_image(input_image_path, output_image_path, target_size)