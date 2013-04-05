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
	logfile.write("\nOutput File(s):\n")
	logfile.flush()
	for filename in os.listdir("out"):
		logfile.write(" * " + filename + "\n")
		logfile.flush()
		os.remove("out/"+filename)
	logfile.write("==================================================\n\n")
	logfile.flush()


test(1, './golfscore')
test(2, './golfscore -asdf')
test(5, './golfscore -hctg testdata/in.txt out')
test(6, './golfscore -h testdata/in.txt out')
test(7, './golfscore -c testdata/in.txt out')
test(8, './golfscore -t testdata/in.txt out')
test(9, './golfscore -g testdata/in.txt out')
test(10, './golfscore -ctg testdata/in.txt out')
test(12, './golfscore -c out')
test(13, './golfscore -c testdata/__INVALID__.txt out')
test(14, './golfscore -h')
test(15, './golfscore -hctg testdata/in.txt out')
test(16, './golfscore -ctg testdata/invalid_in.txt out')
test(17, './golfscore -ctg testdata/invalid_in.txt out/__INVALID_DIR__')
test(18, './golfscore testdata/invalid_in.txt -ctg out/__INVALID_DIR__')
test(19, './golfscore testdata/invalid_in.txt out/__INVALID_DIR__ -ctg')
test(20, './golfscore -ctg testdata/in.txt out extra parameters here')




