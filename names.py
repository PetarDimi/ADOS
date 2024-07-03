import os

def write_image_names_to_txt(input_folder, output_file):
    with open(output_file, 'w') as file:
        for filename in os.listdir(input_folder):
            if filename.endswith(('.jpg', '.jpeg', '.png', '.gif')):
                file.write(f'<in_img file_name="{filename}"/>\n')

# Set the input folder path containing the images
input_folder = 'datasets/resized/'

# Set the output text file path
output_file = 'datasets/resized/imena.txt'

write_image_names_to_txt(input_folder, output_file)