# This skript along with Test used to check correct of BigInteger module
# by comparing result of executing specified operation with random numbers,
# calculated by Python and interactive program using BigInteger.

# Before use need to specify tested operation in command line arguments

import random
import subprocess
import sys
import platform



# numbers of test expression
repeat_count = 1000
# random number in range [-abs_rand_range, abs_rand_rang)
abs_rand_range = 10 ** 1000
test_program_name = "test.exe"

# string with chosen operation 
op_s = ""
# function, that provides chosen operation
op_func = lambda x,y: x+y 



def usage():
	print( "Usage op_test.py {+/-/*}")


def err_message(i,expression, expected, given):
	print( "Falied on   " + str(i) + " iteration")
	print( "Expression: " + str(expression))
	print( "Expected:   " + str(expected))
	print( "Given:      " + str(given))



if platform.system() != "Windows":
	test_program_name = "./" + test_program_name

# parsing operation
if len(sys.argv) == 2:

	if sys.argv[1] == '+':
		op_func = lambda x,y: x + y
	elif sys.argv[1] == '-' :
		op_func = lambda x,y: x - y
	elif sys.argv[1] == '*' :
		op_func = lambda x,y: x * y
	else:
		print("Wrong argument " + sys.argv[1])
		usage()
		sys.exit(1)

	op_s = sys.argv[1]

else:
	usage()
	sys.exit(1)




# open test file
try:
	test_program = subprocess.Popen(test_program_name,stdout=subprocess.PIPE,stdin=subprocess.PIPE,shell=True)
except Exception as e:
	print("Filed:       No test program")
	sys.exit(1)



for i in range(repeat_count):

	# generating numbers
	first_op = random.randrange(-abs_rand_range, abs_rand_range)
	second_op = random.randrange(-abs_rand_range, abs_rand_range)

	# writing operands
	test_program.stdin.write( str(first_op) + '\n')
	test_program.stdin.write( str(second_op) + '\n')


	# specify operation
	test_program.stdin.write( op_s + '\n')


	# read result
	expected_result = op_func(first_op, second_op)
	#result = test_program.stdout.readline()
	result = test_program.stdout.readline()

		# compare results
	if int(result) != expected_result:
		err_message(i, str(first_op) + "\n"+ op_s + " " + str(second_op) , expected_result , result)
		sys.exit(1)


#end program
test_program.kill()

print( "Test successfully passed with " + str(repeat_count) + " iterations." )