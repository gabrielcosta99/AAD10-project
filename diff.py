from collections import defaultdict

def check_duplicate_lines(file_path):
    try:
        # Dictionary to store line content (ignoring the first 4 characters) as keys and their line numbers as values
        line_dict = defaultdict(list)
        
        # Read the file and populate the dictionary
        with open(file_path, 'r') as file:
            for line_num, line in enumerate(file, start=1):
                # Strip whitespace and ignore the first 4 characters
                processed_line = line[4:].strip()
                line_dict[processed_line].append(line_num)
        
        # Check for duplicates
        duplicates_found = False
        for line, occurrences in line_dict.items():
            if len(occurrences) > 1:  # If a line appears more than once
                duplicates_found = True
                print(f"Duplicate line (ignoring first 4 characters): '{line}' appears on lines {', '.join(map(str, occurrences))}")
        
        if not duplicates_found:
            print("All lines in the file are unique.")
    except FileNotFoundError:
        print(f"The file '{file_path}' does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage
file_path = "deti_coins_vault.txt"  # Replace with the path to your file
check_duplicate_lines(file_path)
