######################################################################################
######################################################################################
######################################################################################
# @file zip.py
# @version 1.0.0

# \brief Common Zip operations class file.
# \project Common
# \author Damian Nowok
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr b22656
# @lastmoddate Aug-06-2012

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

class ZipException(rootexception.RootException): pass

class Zip:
    """
        Class Zip is responsible for zipping files.
        
        Variables:
        path            - zip dir
        file_name       - zip file name
        root            - zip root
        ref             - zip file reference
    """

    def __init__(self,path,file_name = None):
        import os
        
        self.ref = None
        
        if file_name == None:
            self.file_name = os.path.basename(path)
            self.path = os.path.dirname(path)
        else:
            self.path = path
            self.file_name = file_name
    
    def wOpen(self):
        """
            This function opens zip file for writing
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        import zipfile
        import os
        try:
            self.ref = zipfile.ZipFile(os.path.join(self.path,self.file_name),'w',zipfile.ZIP_DEFLATED)
        except:
            raise ZipException(os.path.join(self.path,self.file_name) + " file cannot be open.")
    
    def rOpen(self):
        """
            This function opens zip file for reading
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        import zipfile
        import os
        try:
            self.ref = zipfile.ZipFile(os.path.join(self.path,self.file_name),'r',zipfile.ZIP_DEFLATED)
        except:
            raise ZipException(os.path.join(self.path,self.file_name) + " file cannot be open.")
    
    def extractAll(self,path=None,members=None,pwd=None):
        """
            This function opens zip file for reading and 
            extracts all files to path and
            closes archive
            
            Input:
            path    - path where to extract files ( None = Local folder)
            members - What to extract (None = All)
            pwd     - Password (None = no password) 
            
            Retruns:
            Nothing
        """
        from common.log import display
        self.rOpen()
        
        if members is None:
            members = self.ref.namelist()

        for zipinfo in members:
            try:
                self.ref.extract(zipinfo, path, pwd)
            except IOError as error:
                # Triggered when cannont overwrite or write to path
                # It is displayed as warning becasue on most occations
                # this is triggerd by overwrite of read only file which can be accepted!
                # I dont like this as much as you do. Believe me!
                display.output.log(str(error),1)
            except Exception as error:
                raise ZipException(str(error))
            
        self.close()
    
    def write(self,path,root):
        """ 
            This function writes file to zip file.
            Before using this function zip file has to be open with wOpen()
            After zipping is complete zip file has to be closed with close()
            
            Input:
            path    - path to folder/file to be zipped
            root    - zip root
            
            Returns:
            Nothing
        """
        import zipfile
        import os

        if os.path.isfile(path):
            #Add new file to zip
            try:
                self.ref.write(path,root,zipfile.ZIP_DEFLATED)
            except:
                raise ZipException("File " + path + " cannot be write to " + os.path.join(self.path,self.file_name))
        elif os.path.isdir(path):
            #Add new dir to zip
            dir_list = os.listdir(path)
            if root != "": self.createDir(root)
            for entity in dir_list:
                next_zip_root = os.path.join(root,entity)
                next_path = os.path.join(path,entity)
                self.write(next_path,next_zip_root)
        else:
            raise ZipException("Bad file path: " + path)

    def createDir(self,dir):
        """
            This function creates folder inside zip file
            
            Input:
            dir     - folder name
            
            Returns:
            Nothing
        """
        try:
            self.ref.writestr(dir + "/",'')
        except:
            raise ZipException("Cannot write " + dir + " to zip file")
    
    def close(self):
        """
            This function closes zip file and changes zip file reference to None
            
            Input:
            Nothing
            
            Returns:
            Nothing
        """
        if self.ref != None: 
            self.ref.close()
            self.ref = None
        
    def __del__(self):
        self.close()
