######################################################################################
######################################################################################
######################################################################################
# @file keyboard.py
# @version 1.0.0

# \brief Remote Run keyboard input functions.
# \project Remote Run
# \author Damian Nowok
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr b22656
# @lastmoddate Jan-30-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.
######################################################################################
######################################################################################
######################################################################################

######################################################################################
################################### Functions ########################################
######################################################################################

def waitForKey():
    """
        This function waits for keyboard button press
        Returns pressed button
        
        Input:
        Nothing
        
        Return:
        Key value
    """
    import msvcrt 
    
    while msvcrt.kbhit(): pass
    return ord(msvcrt.getwch())
