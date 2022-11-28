#
# Script for running (parallel) HW Simulations of VPRO and execute testcases from verifyHW project
#
# used by: CI/CD (Gitlab) / Jenkins
#

import getpass, os, time, sys, datetime
import subprocess
from junit_xml import TestSuite, TestCase

######################################################################################################
# Configurations
######################################################################################################

# these tests are skipped
# as well as tests beginning with "TEST_"
skip_test = [
    "DMA_FPGA",
    "BIT_REVERSAL",
    "MIN_VECTOR_VAL",
    "MAX_VECTOR_VAL",
    "MIN_VECTOR_INDEX",
    "MAX_VECTOR_INDEX",
    "LOADS_SHIFT_L",
    "LOADS_SHIFT_R",
]

# Docker is working in /builds/ASIP/CORES/...

# containing enum with IDs to handle test by checker/app
header_file = '../CORE_VPRO/sw/vpro/verifyHW/includes/test_framework.h'
generate_new_input_data = False # whether to generate new input binary files for each test... 

# repositories
try:
    SYS_DIR = os.environ['CI_PROJECT_DIR']+"/../SYS/scr/"
    VPRO_DIR = os.environ['CI_PROJECT_DIR']+"/../CORE_VPRO/sw/host/verifyHW/"
except:
    SYS_DIR = "../SYS/scr/"
    VPRO_DIR = "../CORE_VPRO/sw/host/verifyHW/"

# UEMU
system_last_port_file = os.environ['HOME']+"/.uemu_sys_last_port"
port_start_default = 50185 # default start port for UEMU
parallel = 32              # number of test to run parallel in one batch
sim_init_time = 90         # seconds for uemu multi_start to initialize before checker starts

# Screen Sessions (use a config & log file)
work_dir = "/localtemp2/"+getpass.getuser()+"/vpro_verify_tests/"
screen_names = "vpro_silent_test" # + _<#nr>

# Result file for GitLab CI
try:
    junit_test_report_file = os.environ['CI_PROJECT_DIR']+"/report.xml"
except:
    junit_test_report_file = "report.xml"
print("Result Report: ", junit_test_report_file)

# unbuffered!
sys.stdout.flush()

framework_start_time = datetime.datetime.now()
######################################################################################################
# helper

def running_sessions(screen_names):
    return subprocess.run("screen -list | grep "+screen_names+" | wc -l",
                     stdout=subprocess.PIPE,
                     stderr=subprocess.STDOUT, shell=True).stdout.decode('utf-8').replace("\n", "")
                     
def is_port_free(port):
    return (0 == int(subprocess.run('netstat -ln | grep ":'+str(port)+' " | grep "LISTEN" | wc -l',
                     stdout=subprocess.PIPE,
                     stderr=subprocess.STDOUT, shell=True).stdout.decode('utf-8').replace("\n", "")))
    
def is_vsimk_runnning():
    return (0 != int(subprocess.run('ps -u '+getpass.getuser()+' -U '+getpass.getuser()+' | grep vsimk | wc -l',
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT, shell=True).stdout.decode('utf-8').replace("\n", "")))
                     
def is_screen_runnning():
    return (2 != int(subprocess.run('screen -list | wc -l',
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT, shell=True).stdout.decode('utf-8').replace("\n", "")))
                         
def start_test_screens(relevant_test_list, port_tmp = port_start_default):
    for test in relevant_test_list:
    
        # next free port
        while (not is_port_free(port_tmp)):
            print("Port", port_tmp, "already in use by this System!")
            port_tmp += 2

        # config file for screen log
        config="""logfile """+work_dir+"logs/"+screen_names+"_"+str(test)+""".log
logfile flush 1
log on
logtstamp after 1
logtstamp string \"[ %t: %Y-%m-%d %c:%s ]\"
logtstamp on""";
        open(work_dir+"configs/screen_config_"+str(test)+".conf", "w").write(config)

#                    make -C """+test_work_dir+"""/CORE_VPRO/sw/host/verifyHW clean_all compile;
        test_work_dir = work_dir+"instances/"+str(test)+"_"+test_list[test]
        # command to run inside screen
        command ="""set -e; 
                    export DISPLAY=:0;
                    export UEMU_PORT_START="""+str(port_tmp)+""";
                    export UEMU_HOST=localhost;
                    export UEMU_CONN_MODE=SIM_IO;
                    export UEMU_NUM_INSTANCES=1;
                    export UEMU_ETH_INTERFACE=eth4;
                    export UEMU_INSTANCE_DIR=MultiSimPids"""+str(test)+""";
                    export XILINX_DSP=/opt/xilinx/ISE/14.7/14.7/ISE_DS/ISE;
                    export XILINXD_LICENSE_FILE=55008@license1.ims-as.uni-hannover.de;
                    export XILINX_EDK=/opt/xilinx/ISE/14.7/14.7/ISE_DS/EDK;
                    export XILINX_PLANAHEAD=/opt/xilinx/ISE/14.7/14.7/ISE_DS/PlanAhead;
                    export XILINX_common=/opt/xilinx/ISE/14.7/14.7/ISE_DS/common;
                    export XILINX=/opt/xilinx/ISE/14.7/14.7/ISE_DS/ISE;
                    export MTI_VCO_MODE=64;
                    export MTI_HOME=/opt/mentor/questasim/2019.2/questasim;
                    export WORKDIR="""+test_work_dir+""";
                    
                    mkdir -p """+test_work_dir+"""; 
                    rsync -avq --delete """+work_dir+"""backup/* """+test_work_dir+"""/; 
                    """;
        if generate_new_input_data: 
            command = command + """
                    dd if=/dev/urandom of="""+test_work_dir+"""/CORE_VPRO/sw/host/verifyHW/data/opa.bin bs=2 count=64
                    dd if=/dev/urandom of="""+test_work_dir+"""/CORE_VPRO/sw/host/verifyHW/data/opb.bin bs=2 count=64
                    """;
                    
        command = command + """
                    echo -e \\"\n####################################\nInput Operands:\\"
                    echo -e \\"OPA:\\"
                    xxd """+test_work_dir+"""/CORE_VPRO/sw/host/verifyHW/data/opa.bin
                    echo -e \\"OPB:\\"
                    xxd """+test_work_dir+"""/CORE_VPRO/sw/host/verifyHW/data/opb.bin
                    echo -e \\"####################################\n\\"
                    printenv | grep UEMU;
                    echo \\"sleeping for """+str(sim_init_time)+""" Seconds before starting the APP.\\";
                    echo \\"Starting UMEU Instance\\";
                    make -s -C """+test_work_dir+"""/SYS/scr WORKDIR="""+test_work_dir+""" CORE=CORE_VPRO clean_all uemu_dma > /dev/null;
                
                    make -s -C """+test_work_dir+"""/SYS/scr WORKDIR="""+test_work_dir+""" CORE=CORE_VPRO compile multi_start;
                    sleep """+str(sim_init_time)+""";
                    echo 'fail' > """+work_dir+"""results/test_result_"""+str(test)+""";
                    make -C """+test_work_dir+"""/CORE_VPRO/sw/host/verifyHW run_silent SILENT_TEST="""+str(test)+""";
                    if [ $? -eq 0 ]; then
                        echo 'pass' > """+work_dir+"""results/test_result_"""+str(test)+""";
                        echo -e \\"\n###########################################\n Passed!\n##########################################################\n\\";
                    else
                        echo -e \\"\n###########################################\n Failed!\n##########################################################\n\\";
                    fi;
                    sleep 5;
                    make -s -C """+test_work_dir+"""/SYS/scr WORKDIR="""+test_work_dir+""" CORE=CORE_VPRO multi_stop;
                    sleep 5;
                    echo -e \\"Run done. exiting\\"
                 """
        
        # start screen session
        print("screen starting test... ", test, ":", test_list[test], "Port:", port_tmp, end="")
        os.system("screen -L -c "+work_dir+"configs/screen_config_"+str(test)+".conf -m -d -S "+screen_names+"_"+str(test)+" bash -c \""+command+"\"")

        with open(system_last_port_file, "w") as port_file:
            port_file.write(str(port_tmp))
        port_tmp += 2
#        time.sleep(1)

        remaining_sessions = int(running_sessions(screen_names))
        if remaining_sessions >= parallel:
            print("")
            while remaining_sessions >= parallel:
                print("\r\tRunning parallel screen sessions: "+str(remaining_sessions)+" [Sleep 10s]", end="")
                print("") # newline (comment if log gets too long)
                try:
                    time.sleep(10)
                    remaining_sessions = int(running_sessions(screen_names))
                except:
                    print("Determine running screen sessions caused error! - should be 0 or above!")
                    print("Unexpected error:", sys.exc_info()[0])
                    print(remaining_sessions)
                    remaining_sessions = 0
        print("")
    
######################################################################################################
# prepare folders and variables

with open(system_last_port_file, "w") as port_file:
    port_file.write(str(port_start_default))
    
try:
    try:
        port_current = int(open(system_last_port_file, "r").readlines())
    except:
        port_current = port_start_default
except IOError: 
    # if no system port last file exists (no pipeline / fresh system) use default start port
    os.system("touch "+system_last_port_file)
    port_current = port_start_default
    
os.system("rm -rf " + work_dir+"")
os.system("mkdir -p " + work_dir + "configs")
os.system("mkdir -p " + work_dir + "results")
os.system("chmod 777 " + work_dir + "results")
os.system("mkdir -p " + work_dir + "logs")

# used by rsync to copy relevant files
includes="""
+ SYS
+ scr/
+ scr/**
+ tools/
+ tools/**
+ CORE_VPRO
+ rtl/
+ rtl/**
+ sw/
+ sw/isa*/
+ sw/isa*/**
+ sw/vpro/
+ sw/vpro/common/
+ sw/vpro/common/**
+ sw/vpro/verifyHW/
+ sw/vpro/verifyHW/**
+ sw/vpro/ref_cmakeList.txt
+ sw/host/
+ sw/host/common/
+ sw/host/common/**
+ sw/host/verifyHW/
+ sw/host/verifyHW/data/
+ sw/host/verifyHW/data/**
+ sw/host/verifyHW/data/***
+ sw/host/verifyHW/**
+ sw/host/verifyHW/***
+ sw/helper/
+ sw/helper/**
+ sw/bin/
+ sw/bin/**
""";
open(work_dir+"configs/rsync_include.txt", "w").write(includes)

print("Generating executable.bin")
os.system("make -s -C ../CORE_VPRO/sw/host/verifyHW clean_all compile")

print("Generating Random Input Data")
os.system(" ")
os.system("dd if=/dev/urandom of=../CORE_VPRO/sw/host/verifyHW/data/opa.bin bs=2 count=64")
os.system("dd if=/dev/urandom of=../CORE_VPRO/sw/host/verifyHW/data/opb.bin bs=2 count=64")
if not generate_new_input_data:
    print("    [In Data Gonna be the same for all tests]")

# RSYNC initial copy
print("Copying files to "+work_dir+"/backup")
os.system("mkdir -p "+work_dir+"backup")
os.system("rsync -a --delete ../CORE_VPRO ../SYS "+work_dir+"/backup/ --include-from "+work_dir+"configs/rsync_include.txt --exclude '*';")
os.system("ls -al "+work_dir+"/backup")
# find $WORKDIR/backup/APP_HA -type d -name "out" -exec rm -rf {} +

    
######################################################################################################
# Read Header of test_framework to get ID for all testcases

from pyparsing import *

LBRACE, RBRACE, EQ, COMMA, HASHTAG = map(Suppress, "{}=,#")
_enum = Suppress("enum")
identifier = Word(alphas, alphanums + "_")
integer = Word(nums)
enumValue = Group(identifier("name") + Optional(EQ + integer("value")))
enumList = Group(enumValue + Optional(ZeroOrMore(COMMA + enumValue)))
enum = _enum + identifier("enum") + LBRACE + enumList("names") + Optional(COMMA) + RBRACE
enum.ignore(cppStyleComment)
enum.ignore(cStyleComment)
#enum.ignore(pythonStyleComment)
test_list = {} # found enum (map, index is id)

# find instances of enums ignoring other syntax
for item, start, stop in enum.scanString(open(header_file, 'r').read()):
    id = 0
    for entry in item.names:
        if entry.value != "":
            id = int(entry.value)
        test_list[id] = entry.name.upper()
#        print("%s = %d" % (entry.name.upper(), id)) # enum: item.enum.upper()
        id += 1

relevant_test_list = {}
# Relevant Tests (not beginnning with TEST_)
for test in test_list:
    if test_list[test].startswith("TEST_"):
        continue
    if test_list[test] in skip_test:
        continue
    relevant_test_list[test] = test_list[test]
    
# Test list is generated

#relevant_test_list = {}
#relevant_test_list[66] = "ABS"
#relevant_test_list[65] = "FAIL" # group...

######################################################################################################
# print tests

print("Relevant Tests: ")
for test in relevant_test_list:
    print(test, ": ", relevant_test_list[test])
print("\n")

try: # except Ctrl-C
######################################################################################################
#SIM: make clean_all compile 
#HOST: make compile

    if int(running_sessions(screen_names)) > 0:
        print("There are still Screen sessions running [check manual!]")
        os._exit(1)
        
    os.system('make -s -C '+SYS_DIR+' CORE=CORE_VPRO multi_stop;')

        
######################################################################################################
#SIM: make multi_start
#HOST: make run_silent test
            
    start_test_screens(relevant_test_list, port_tmp = port_current)

    print("All relevant test have been started! ") 
    start_time = datetime.datetime.now()
    remaining_sessions = int(running_sessions(screen_names))   
    while remaining_sessions > 0:
        print("\r\tRemaining running parallel screen sessions: "+str(remaining_sessions)+" [Sleep 10s]", end="")
        try:
            time.sleep(10)
            remaining_sessions = int(running_sessions(screen_names))
        except:
            print("")
            print("Determine running screen sessions caused error! - should be 0 or above!")
            print("Unexpected error:", sys.exc_info()[0])
            print(remaining_sessions)
            remaining_sessions = 0
        elapsed_seconds = (datetime.datetime.now() - start_time).seconds
        if elapsed_seconds > 500:
            print(" Forced Quit after 500s! - maybe session is still running (error?)")
            os.system("screen -list | grep "+screen_names+"")
            break;
        if elapsed_seconds > 200:
            print(" Time is running out. > 200s already: ", elapsed_seconds, "s [max 500s]", end="")
        print("") # newline (comment if log gets too long)
    print("")
    
    ######################################################################################################
    # run sim + test again if no result file has been generated
    second_try_list = {}
    for test in relevant_test_list:
        try:
            result = open(work_dir+"results/test_result_"+str(test), "r").read()
            print("Test: ",test, " ", relevant_test_list[test], " has a result...: ", result.replace("\n",""))
        except: # no results?
            second_try_list[test] = relevant_test_list[test]
            print("Gonna repeat Test: ", test, " ", relevant_test_list[test])
    if len(second_try_list) > 0:
        print(len(second_try_list), "Tests had no result (neither pass nor fail). Start those again in Screens...")
        try:
            port_current = int(open(system_last_port_file, "r").readlines())
        except:
            port_current = port_start_default
        start_test_screens(second_try_list, port_tmp = port_current)
        print("Started those again in Screens...") 
        start_time = datetime.datetime.now()
        remaining_sessions = int(running_sessions(screen_names))   
        while remaining_sessions > 0:
            print("\r\tRemaining running parallel screen sessions: "+str(remaining_sessions)+" [Sleep 10s]", end="")
            try:
                time.sleep(10)
                remaining_sessions = int(running_sessions(screen_names))
            except:
                print("")
                print("Determine running screen sessions caused error! - should be 0 or above!")
                print("Unexpected error:", sys.exc_info()[0])
                print(remaining_sessions)
                remaining_sessions = 0
            elapsed_seconds = (datetime.datetime.now() - start_time).seconds
            if elapsed_seconds > 500:
                print(" Forced Quit after 500s! - maybe session is still running (error?)")
                os.system("screen -list | grep "+screen_names+"")
                break;
            if elapsed_seconds > 200:
                print(" Time is running out. > 200s already: ", elapsed_seconds, "s [max 500s]", end="")
            print("") # newline (comment if log gets too long)
        print("")
        
    ######################################################################################################
    print("Reading Test Results... ")    
    # read results + eval
    # generate JUnit Result file to be included to GitLab

    fails = 0
    test_cases = []
    for test in relevant_test_list:
        elapsed_seconds = (datetime.datetime.now() - framework_start_time).seconds
        case = TestCase('Test: '+relevant_test_list[test], classname=relevant_test_list[test], elapsed_sec=elapsed_seconds)
        test_cases.append(case)
        try:
#            os.system("cat "+work_dir+"results/test_result_"+str(test))
#            os.system("cat "+work_dir+"logs/"+screen_names+"_"+str(test)+".log")
            result = open(work_dir+"results/test_result_"+str(test), "r").read()
            print("Nr.:", test, "Test:", relevant_test_list[test], "Result:", str(result.replace("\n","")))
            if "fail" in result:
                fails += 1
                try:
                    try:
                        with open(VPRO_DIR+"data/opa.bin", "rb") as opa:
                            opa_string = "OPA:\n" + opa.read().hex()
                    except:
                        opa_string = "OPA: "+str(sys.exc_info()[0])  
                    try:
                        with open(VPRO_DIR+"data/opb.bin", "rb") as opb:
                            opb_string = "OPB:\n" + opb.read().hex()
                    except:
                        opb_string = "OPB: "+str(sys.exc_info()[0])  
                    try:
                        with open(VPRO_DIR+"data/result.bin", "rb") as res:
                            res_string = "Result:\n" + res.read()
                    except:
                        res_string = "Result: "+str(sys.exc_info()[0])
                    try:
                        with open(work_dir+"logs/"+screen_names+"_"+str(test)+".log", "r") as log:
                            log_string = "Log:\n" + log.read()
                    except:
                        log_string = "Log: "+str(sys.exc_info()[0])
                    case.add_error_info(message="fail", output=opa_string + opb_string + res_string + log_string)
                except:
                    case.add_error_info(message="fail", output=str(sys.exc_info()[0]))
                os.system("tail -n 100 "+work_dir+"logs/"+screen_names+"_"+str(test)+".log")
        except:
            print("Error")
            print("Unexpected error:", sys.exc_info()[0])
            print("Nr.:", str(test), "Test:", str(relevant_test_list[test]), "No Result")
            with open(work_dir+"logs/"+screen_names+"_"+str(test)+".log", "r") as log:
                case.add_skipped_info(message="no results", output=str(sys.exc_info()[0]) + "\n\n" + log.read())
            os.system("tail -n 100 "+work_dir+"logs/"+screen_names+"_"+str(test)+".log")

    ts = TestSuite("[VPRO] HW Verification Tests", test_cases)
    print(TestSuite.to_xml_string([ts]))
    
    with open(junit_test_report_file, 'w') as f:
        TestSuite.to_file(f, [ts], prettyprint=False)
    
    ######################################################################################################
    # all test done. return code of this framework
    if fails != 0:
        print("There have been errors in", fails, "Tests!")
        os._exit(1) 
    else:
        print("All Tests run successful!")
        os._exit(os.EX_OK) 
    
except KeyboardInterrupt:
    print("\n\nExiting... [Killing Screens: ", end="")
    for test in relevant_test_list:
        if int(running_sessions(screen_names+"_"+str(test))) > 0:
            os.system("screen -S "+screen_names+"_"+str(test)+" -X quit")
            print(test, " ", end="")
    print(" | killing remaining VSIMK instances]")
    os.system('make -C '+SYS_DIR+' CORE=CORE_VPRO multi_stop;')
    time.sleep(1)
    os.system('killall vsimk')
    os._exit(os.EX_OK) 

