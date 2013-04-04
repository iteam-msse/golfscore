import os, subprocess

os.system('g++ main.cpp -o golfscore')

test_log = open('results.out', 'a')

def test(test_number, cmd, logfile=test_log):
	logfile.write("==================================================\n")
	logfile.write("Test Case: " + str(test_number) + "\n")
	logfile.write("Output:\n\n")
	logfile.flush();
	p = subprocess.Popen(cmd, shell=True, universal_newlines=True, stdout=logfile, stderr=logfile)
	ret_code = p.wait()
	logfile.flush()
	logfile.write("\nResult Code: " + str(ret_code) + "\n")
	logfile.write("==================================================\n\n")
	logfile.flush()


test(1, './golfscore')
test(2, './golfscore -h')