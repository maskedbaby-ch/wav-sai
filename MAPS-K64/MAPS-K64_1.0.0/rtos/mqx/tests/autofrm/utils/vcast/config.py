######################################################################################
######################################################################################
######################################################################################
# @file config.py
# @version 1.0.0

# \brief VectorCAST - Code coverage analysis - Configuration
# \project VectorCAST
# \author Vaclav Hamersky
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr B21889
# @lastmoddate September-11-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.
######################################################################################
######################################################################################
######################################################################################

# In place
IN_PLACE='Y'

# Config and command file for VectorCAST
VC_CFG_FILE='CCAST_.CFG'
VC_LOG_FILE='vcast.log'

# File holding all include paths
INCLUDE_PATHS_LIST='include_paths.lst'
# Template for VectorCAST configuration file
TEMPLATE_FILE='vcast_cnfg.tmpl'

# list of files that can NOT be instrumented because they contains function called before global data are zero-ed
FORBIDEN_FILES=[ 'init_hw.c', 'bsp_cm.c', 'comp.c' ]

REPORT_FILES = ['Aggregate.html', 'Ram_usage.csv', 'MAnagement.html', 'Basis.txt', 'Csv_metrics.csv',
                'Unit_metrics.html', 'Mcdc.html', 'MCDC_Equivalence.html', 'MCDC_Test.txt']
REPORT_COMMAND = 'Cover Report'
REPORT_DIR = 'reports'
VCAST_COMMAND = '"%VECTORCAST_DIR%\CLICAST" /E:'
