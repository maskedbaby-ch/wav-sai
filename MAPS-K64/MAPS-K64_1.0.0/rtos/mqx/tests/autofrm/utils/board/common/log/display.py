######################################################################################
######################################################################################
######################################################################################
# @file display.py
# @version 1.0.0

# \brief Display class file that controls console output and log files.
# \project Common
# \author Damian Nowok
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr b22656
# @lastmoddate May-31-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.
######################################################################################
######################################################################################
######################################################################################
from common.exception import rootexception  

######################################################################################
################################### Classes ##########################################
######################################################################################

class DisplayException(rootexception.RootException): pass

class Display:
    """
        This class is responsible for displaying information
        on console or writing it to file.
        
        Constants:
        SHOW_LOG                    - Log level
        TIME_FORMAT                 - Time format
        MAX_LOG_ENTRIES_IN_MEMORY   - Defines how many log entires should be kept in memory.
        EXIT                        - Queue organization name
        LOG                         - Queue organization name
        
        Variables:
        log_history_mutex           - History Log insert mutex
        displayer_queue             - Communication queue for displayer
        last_message                - Last displayed message
        log_history                 - log history
        displayer_thread            - Displayer thread reference
        style                       - Display colors style
        external_queue              - External queue used for multiprocessing
        external_queue_thread       - Monitor for external queue
        logs_path                   - Path to log file
        logs_to_console             - Log to console
    """
    # What to log list: 0 - Info (1 level), 1 - Warning (1 level), 2 - Error (1 level), 3 - No severity (1 level), 4 - Info (2 level), 5 - Warning (2 level), 6 - Error (2 level), 7 - Network input, 8 - Network output
    SHOW_LOG = [0,1,2,3]
    TIME_FORMAT = "%Y-%m-%d %H:%M:%S"
    MAX_LOG_ENTRIES_IN_MEMORY = 200
    EXIT = "EXIT"
    LOG = "LOG"
    
    def __init__(self,logs_path=None, logs_to_console=True):
        import threading
        import multiprocessing
        import Queue
        from common import colorama
        
        # Console style definitions
        self.NORMAL = ""
        self.RED = colorama.Fore.RED + colorama.Style.BRIGHT
        self.GREEN = colorama.Fore.GREEN + colorama.Style.BRIGHT
        self.YELLOW = colorama.Fore.YELLOW + colorama.Style.BRIGHT
        
        self.logs_to_console = logs_to_console
        self.logs_path = logs_path
        self.last_message = ""
        self.log_history = []
        self.log_history_mutex=threading.RLock()
        self.displayer_queue = Queue.Queue(0)
        
        self.displayer_thread = threading.Thread(target=self._displayer)
        self.displayer_thread.daemon = True
        self.displayer_thread.start()
        
        self.external_queue = multiprocessing.Queue()
        self.external_queue_thread = threading.Thread(target=self._external_queue)
        self.external_queue_thread.daemon = True
        self.external_queue_thread.start()
        
        colorama.init()
        self.style = self.NORMAL

    def _external_queue(self):
        """
            This function monitors external queue.
            Other python processes can use it to write data to displayer
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        while True:
            data = self.external_queue.get()
            if isinstance(data,_Element) and data.type == self.EXIT: break
            if isinstance(data,str): self.log(data)
            if isinstance(data,tuple) and len(data) == 2: self.log(data[0],data[1])

    def _displayer(self):
        """
            This function is a thread that displays logs on console.
            Only this function should display things on console.
            Thanks to that mouse click on console will not stop execution.
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        while True:
            element = self.displayer_queue.get()
            if element.type == self.LOG:
                style,output_string = element.value
                if self.logs_to_console == True:
                    print style + output_string

                if self.logs_path != None:
                    fp = open(self.logs_path,"a")
                    fp.write(output_string+"\n")
                    fp.close()
            
            elif element.type == self.EXIT: break
                    
    def log(self,message,severity=3):
        """ Appends log to log queue
            
            Input:
            message - Message to be logged
            severity - Message level (0 - info, 1 - error, 2 - warning)
            
            Retrun:
            Nothing
        """
        from time import localtime, strftime
        
        if message == self.last_message: return
        
        if not severity in self.SHOW_LOG: return
        
        self.last_message = message
        
        output = strftime(self.TIME_FORMAT, localtime()) + " ### "
        
        if severity == 0 or severity == 4:
            output+="INFO: " + message
        elif severity == 1 or severity == 5:
            output+="WARNING: " + message
        elif severity == 2 or severity == 6:
            output+="ERROR: " + message
        elif severity == 7:
            output+="RECEIVED: " + message
        elif severity == 8:
            output+="SEND: " + message
        else:
            output+=message
        
        self.log_history_mutex.acquire()
        self.log_history.append(output)
        if len(self.log_history) >= self.MAX_LOG_ENTRIES_IN_MEMORY:
            self.log_history.pop(0)
        self.log_history_mutex.release()
        
        self.displayer_queue.put(_Element(self.LOG,(self.style,output)))
    
    def getHistory(self):
        """
            This function returns log history
            
            Input:
            Nothing
            
            Returns:
            Log history
        """
        self.log_history_mutex.acquire()
        list_copy = self.log_history[:]
        self.log_history_mutex.release()
        return list_copy
    
    def signalExit(self):
        """
            This function sends signal exit.
            
            Input:
            Nothing
            
            Return:
            Nothing
        """
        self.displayer_queue.put(_Element(self.EXIT,True))
        self.external_queue.put(_Element(self.EXIT,True))

    def exited(self):
        """
            This function checks if exited
            
            Input:
            Nothing
            
            Returns:
            True if exited, Flase if not exited.
        """
        return (not self.displayer_thread.is_alive()) and (not self.external_queue_thread.is_alive())
    
    def displayerIsRunning(self):
        """
            Checks if displayer thread is running
            
            Input:
            Nothing
            
            Returns:
            True/False
        """
        return self.displayer_thread.is_alive() and self.external_queue_thread.is_alive()
        
    def status(self):
        """
            Returns displayer status
            
            Input:
            Nothing
            
            Returns:
            True/False
        """
        status = {}
        status["Consol output thread"] = self.displayer_thread.is_alive() and self.external_queue_thread.is_alive()
        return status
        
    def unbuffered(self):
        """
            This function turns off stdout and stderr buffering
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        import sys
        import os

        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
        sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 0)
    
    def buffered(self):
        """
            This function turns on stdout and stderr buffering
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        import sys
        
        sys.stdout = sys.__stdout__
        sys.stderr = sys.__stderr__

class _Element:
    """
        This class is data structure used to communicate with Displayer
        
        Variables:
        type    - Type of value
        value   - Value contents
    """
    
    def __init__(self,type,value):
        self.type = type
        self.value = value

# If global object output does not exists it is being created
try:
    output
except:
    output = Display();

    