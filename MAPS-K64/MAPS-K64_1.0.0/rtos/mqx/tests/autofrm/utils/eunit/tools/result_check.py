import os, argparse

def createCookieFile(file_name):
    """
        This file will create .mak file included by 'tapp_settings.mak'
        Variable TEST_TYPE = LINK_WITH_ERROR will suppress make's run

        Input:
        file_name   - full path to suppress_run.mak file
    """

    disable_rerun_string = "TEST_TYPE:=LINK_WITH_ERROR\n"

    with open(file_name, 'a') as f:
        f.write(disable_rerun_string)


def prepareRerun(debug_dir, err_message):
    """
        This function is called upon error
        prepareRerun back-ups all files (not directories) in 'debug' directory
        into 'run_x' directory, where x is the runs ID (counter starting from 0)

        Input:
        debug_dir     - 'debug' directory, where to look for files to back-up
        err_message   - message informing what went worng
    """
    import re, shutil

    print err_message
    print "Preparing rerun"

    high_idx = -1
    rerun_dir_regex = re.compile('^run_(\d+)$')
    files = os.listdir(debug_dir)
    dirs = [d for d in files if os.path.isdir(os.path.join(debug_dir,d))]
    files = [f for f in files if os.path.isfile(os.path.join(debug_dir,f))]

    # find highest x in 'run_x' directories
    for d in dirs:
        search = rerun_dir_regex.search(d)
        if search != None:
            high_idx = max(high_idx, int(search.groups()[0]))

    high_idx += 1
    dest_dir = os.path.join(debug_dir, "run_" + str(high_idx))
    os.makedirs(dest_dir)

    for f in files:
        shutil.copy2(os.path.join(debug_dir,f), os.path.join(dest_dir,f))

    exit(0)


if __name__ == '__main__':
    """
        Count number of test cases described in 'eunit.tst'
        Check if all test cases were executed
        Chack if all asserts didn't fails
        Upon error calls prepareRerun to back-up debug information
        If everything is OK it calls createCookieFile which will suppress next re-run (if launched)
    """
    cookie_file_name ="suppress_run.mak"
    list_file_name = "eunit.tst"
    result_file_name = "result.log"

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Simple test result checker')
    parser.add_argument('-d','--debug_dir', metavar='<debug dir>', type=str, dest='debug_dir', required=True, help='Test result location.')
    args = parser.parse_args()

    # check input data
    cookie_file = os.path.join(args.debug_dir, cookie_file_name)
    if os.path.isfile(cookie_file):
        os.remove(cookie_file)

    result_file = os.path.join(args.debug_dir, result_file_name)
    if not os.path.exists(result_file):
        print "Result file: " + result_file + " doesn't exists"
        exit(1)

    list_file = os.path.join(args.debug_dir, list_file_name)
    if not os.path.exists(list_file):
        print "List file: " + list_file + " doesn't exists"
        exit(2)

    # tc_count will host number of test cases to-be-executed
    tc_count = 0
    try:
        for line in open(list_file, 'r'):
            # lines starting with '+' in 'eunit.tst' identifies test cases
            if line.startswith('+'):
                tc_count += 1
    except Exception as error:
        prepareRerun(args.debug_dir, "Error reading list file")

    if tc_count == 0:
        prepareRerun(args.debug_dir, "No test cases found in list file")

    for line in open(result_file, 'r'):
        # first tc_count number of lines in 'result.log' are describing test case execution
        if tc_count > 0:
            tc_count -= 1
            if not line.startswith('1'):
                prepareRerun(args.debug_dir, "Fault found in test case")
        # rest of 'result.log' is for test asserts
        else:
            if line.startswith('2') or line.startswith('3'):
                prepareRerun(args.debug_dir, "Fault found in assert results")

    if tc_count > 0:
        prepareRerun(args.debug_dir, "All test cases were not covered in result file")

    createCookieFile(cookie_file)

