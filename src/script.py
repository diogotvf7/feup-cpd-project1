import subprocess
import os

# Run for 3 times each instance and get the average
#def run(process, algorithm_n, matrix_size, block_size = 1):
#    results = []
#    for i in range(3):
#        result = run_inst(process, algorithm_n, matrix_size, block_size)
#        result = result.replace("\\n", "")
#        result2 = result.split(",")
#        results.append(result2)#
#
#    avg_results = []
#    for i in range(len(results[0])):
#        print(results)
#        avg_results.append(sum([float(result[i]) for result in results]) / len(results))
#    return avg_results

def run(process, algorithm_n, matrix_size, block_size = 1):
    for line in iter(process.stdout.readline, b''):
        print(line, end="")
        if "Selection: " in line:
            process.stdin.write(f"{algorithm_n}\n")
        elif "Matrix Dimensions: " in line:
            process.stdin.write(f"{matrix_size}\n")
        elif "Block Size: " in line:
            process.stdin.write(f"{block_size}\n")
        elif "Results:" in line:
            result = process.stdout.readline() #!!! This result is a string with , as separator
            process.stdin.flush()
            return result

        process.stdin.flush()
    return None
    

if __name__ == "__main__":
    cpp_path = './matrixproduct'
    julia_path = "matrixproduct-papi.jl"
    paths = [cpp_path, julia_path]

    # Check if programs exist
    for path in paths:
        if not os.path.exists(path):
            print(f"Binary path {path} does not exist.")
            exit(1)

    programs = [[cpp_path], ["julia", julia_path]]

    
    # To register
    # 1. 600x600 to 3000x3000 with 400 intervals
    # 2. 600x600 to 3000x3000 with 400 intervals
    # 2. 4096x4096 to 10240x10240 with 2048 intervals (C++ only)
    # 3. 4096x4096 to 10240x10240 with 2048 intervals 128 block size (C++ only)
    # 3. 4096x4096 to 10240x10240 with 2048 intervals 256 block size (C++ only)
    # 3. 4096x4096 to 10240x10240 with 2048 intervals 512 block size (C++ only)

    for i, program in enumerate(programs):
        program_results_ex1_1 = [] #ex1: 600:3000 with 400 step
        program_results_ex2_1 = [] #ex2: 600:3000 with 400 step
        
        program_results_ex2_2 = [] #ex2: 4096:10240 with 2048 step
        program_results_ex3_1 = [] #ex3: 4096:10240 with 2048 step and 128 block size
        program_results_ex3_2 = [] #ex3: 4096:10240 with 2048 step and 256 block size
        program_results_ex3_3 = [] #ex3: 4096:10240 with 2048 step and 512 block size

        languages = ["C++", "Julia"]
        print(f"Testing language: {languages[i]}")
        process = subprocess.Popen(program, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        for j in range(600, 3400, 400): #change to 3000
            result = run(process, 1, j)
            print(result)
            program_results_ex1_1.append([result])

            result2 = run(process, 2, j)
            program_results_ex2_1.append([result2])

        with open(f"results/{languages[i]}_ex1_1.csv", "w") as file:
            file.write("Matrix Size,Time,DCM1,DCM2,TOT_INS,TOT_CYC\n")
            for result in program_results_ex1_1:
                file.write(",".join(result))
        with open(f"results/{languages[i]}_ex2_1.csv", "w") as file:
            file.write("Matrix Size,Time,DCM1,DCM2,TOT_INS,TOT_CYC\n")
            for result in program_results_ex2_1:
                file.write(",".join(result))

        for j in range(4096, 12288, 2048):

            print("Matrix size: ", j, "2nd algorithm")
            result2 = run(process, 2, j)
            program_results_ex2_2.append([result2])

            print("Matrix size: ", j, "block size: 128")
            result3 = run(process, 3, j, 128)
            program_results_ex3_1.append([result3])

            print("Matrix size: ", j, "block size: 256")
            result4 = run(process, 3, j, 256)
            program_results_ex3_2.append([result4])

            print("Matrix size: ", j, "block size: 512")
            result5 = run(process, 3, j, 512)
            program_results_ex3_3.append([result5])

        with open(f"results/{languages[i]}_ex2_2.csv", "w") as file:
            file.write("Matrix Size,Time,DCM1,DCM2,TOT_INS,TOT_CYC\n")
            for result in program_results_ex2_2:
                file.write(",".join(result))
        with open(f"results/{languages[i]}_ex3_1.csv", "w") as file:
            file.write("Matrix Size,Time,DCM1,DCM2,TOT_INS,TOT_CYC\n")
            for result in program_results_ex3_1:
                file.write(",".join(result))
        with open(f"results/{languages[i]}_ex3_2.csv", "w") as file:
            file.write("Matrix Size,Time,DCM1,DCM2,TOT_INS,TOT_CYC\n")
            for result in program_results_ex3_2:
                file.write(",".join(result))
        with open(f"results/{languages[i]}_ex3_3.csv", "w") as file:
            file.write("Matrix Size,Time,DCM1,DCM2,TOT_INS,TOT_CYC\n")
            for result in program_results_ex3_3:
                file.write(",".join(result))

    # Close the process
    process.stdin.close()
    process.stdout.close()
    process.stderr.close()
    process.terminate()
