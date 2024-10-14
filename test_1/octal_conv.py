import sys
import re

def octal_to_hex(octal_string):
    try:
        # Extract octal values from the input string
        octal_values_list = re.findall(r'\\([0-7]{1,3})', octal_string)
        # Convert each octal value to its corresponding hex representation
        hex_values = ' '.join([hex(int(oct_val, 8)) for oct_val in octal_values_list])
        return hex_values
    except ValueError:
        return "Error: Please make sure all inputs are valid octal numbers."

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python octal_to_hex_converter.py \"<octal_string>\"")
        print("Example: python octal_to_hex_converter.py \"\\344\\314\\377\\200\"")
    else:
        octal_string = sys.argv[1]
        result = octal_to_hex(octal_string)
        print(result)
