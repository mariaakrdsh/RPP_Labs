from mpi4py import MPI
import time


def is_palindrome(s):
    """Check if a string is a palindrome."""
    return s == s[::-1]


def read_input_file(file_name):
    """Read lines from a file and return a list of strings."""
    with open(file_name, 'r') as f:
        strings = [line.strip() for line in f.readlines()]
    return strings


def write_output_file(file_name, palindromes):
    """Write the found palindromic strings to an output file."""
    with open(file_name, 'w') as f:
        for s in palindromes:
            f.write(f"{s}\n")


def distribute_strings(strings, size):
    """
    Distribute strings among all MPI processes. This method divides the list of strings
    into nearly equal chunks for each process.

    Args:
        strings (list): List of strings to be distributed.
        size (int): Number of processes.

    Returns:
        list of list: Each sublist corresponds to the strings assigned to one process.
    """
    chunk_size = len(strings) // size  # Determine the base size of each chunk
    chunks = [strings[i * chunk_size: (i + 1) * chunk_size] for i in range(size)]
    # Handle the remainder if the list size isn't perfectly divisible by the number of processes
    remainder = len(strings) % size
    if remainder:
        for i in range(remainder):
            chunks[i].append(strings[-(remainder - i)])
    return chunks


def main(input_file, output_file):
    start_time = time.time()

    # Initialize MPI environment
    comm = MPI.COMM_WORLD
    size = comm.Get_size()  # Total number of processes in the communicator
    rank = comm.Get_rank()  # Rank of the current process

    strings_to_check = None
    if rank == 0:
        # Root process reads input and distributes the tasks
        strings = read_input_file(input_file)
        chunks = distribute_strings(strings, size)
    else:
        # Other processes do not initially have any strings to check
        chunks = None

    # Scatter the string chunks to all processes
    strings_to_check = comm.scatter(chunks, root=0)

    # Each process checks its own subset of strings
    local_palindromes = [s for s in strings_to_check if is_palindrome(s)]

    # Gather all palindromic strings found by all processes back at the root
    all_palindromes = comm.gather(local_palindromes, root=0)

    if rank == 0:
        # Flatten the list of palindromes and write them to the output file
        all_palindromes = [s for sublist in all_palindromes for s in sublist]
        write_output_file(output_file, all_palindromes)
        print("Palindromic strings found and written to the output file.")
        end_time = time.time()
        total_time_ms = (end_time - start_time) * 1000
        print(f"Total execution time: {total_time_ms:.2f} ms")


if __name__ == "__main__":
    input_file = "C:\\input.txt"
    output_file = "C:\\output.txt"
    main(input_file, output_file)
