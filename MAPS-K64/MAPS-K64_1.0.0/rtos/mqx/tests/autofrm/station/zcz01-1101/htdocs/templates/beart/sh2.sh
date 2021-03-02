#export OUTPUT_DIR=E:/ccwork/b37642_first/MSGSW/MQX/src/app
# export DEV_DIR=E:\\CCWORK\\B37642~1\\MSGSW\\MQX\\SRC

# ------------------------------------------------------------------
# Tools setting
# ------------------------------------------------------------------
export CW10_DIR=C:/PROGRA~1/FREESC~1/CWMCUV~1.1
export CW92_DIR=C:/PROGRA~1/FREESC~1/CODEWA~1.2
export IARARM_DIR=C:/PROGRA~1/IARSYS~1/EMBEDD~1.0
export UV4_DIR=C:/PROGRA~1/KEIL
export PE_DIR=C:/pemicro
export DEV_DIR=E:/ccwork/b37642_first/MSGSW/MQX/src
export FRM_DIR=E:/ccwork/b37642_first/vnv_dist
export CFG_MAK_FILE=../../../MSGSW/MQX/src/tests/autofrm/beart/config_beart.mak


export SH2_OUTPUT_DIR=E:/ccwork/b37642_first/MSGSW/MQX/src/app


### ------------------------------------------------------------------
### platform setting
### ------------------------------------------------------------------

# SH2_PLATFORM='twrpxs20'
# SH2_PLATFORM='twrk60n512'
# SH2_PLATFORM='twrk70f120m'
# SH2_PLATFORM='twrmcf51cn'
SH2_PLATFORM='twrpxs20'
# SH2_PLATFORM='twrmpc5125'


### ------------------------------------------------------------------
### storage settings, depend on [platform]/[tool]/build_cfg.mak
### ------------------------------------------------------------------

# SH2_LOAD_TO=INTRAM
SH2_LOAD_TO=INTFLASH
# SH2_LOAD_TO=EXTRAM


### ------------------------------------------------------------------
### syntax: Capital, UPPERCASE 
### because beart use UPPERCASE, generator use Capital
### ------------------------------------------------------------------

SH2_MODES=(
'Debug'     'DEBUG'
# 'Release'   'RELEASE'
)

### ------------------------------------------------------------------
### syntax: sw debugger, hw debugger, make generator, ide generator
### ------------------------------------------------------------------

SH2_OPTIONS=(
# 'iar'       'iar'       'makeiararm'      'iararm'
'cw10'      'cw10'      'makecw10'        'cw10'
# 'uv4'       'uv4'       'makeuv4'         'uv4'
# 'cw'        'cw92'      'makecwmpc92'     'cwmpc92'
)

### ------------------------------------------------------------------
### syntax: module, test 
### ------------------------------------------------------------------

SH2_MODULES=(
# 'block'                 'block_block'
# 'bsp'                   'bsp_ticks'
# 'bsp'                   'bsp_time'
# 'bsp'                   'bsp_usec1'
# 'bsp'                   'bsp_usec2'
# 'bsp'                   'bsp_usec3'
# 'event'                 'event_event1'
# 'event'                 'event_event2'
# 'fio'                   'fio_fio'
# 'float'                 'float_float0'
# 'float'                 'float_float1'
# 'float'                 'float_float2'
# 'float'                 'float_float3'
# 'float'                 'float_float4'
# 'int'                   'int_int'
# 'int_psp'               'int_psp_int_psp'
# 'ipsum'                 'ipsum_ipsum'
# 'linker'                'linker_mem'
# 'log'                   'log_log1'
# 'log'                   'log_log2'
# 'lwevent'               'lwevent_lwevent1'
# 'lwevent'               'lwevent_lwevent1'
# 'lwlog'                 'lwlog_lwlog'
# 'lwmem'                 'lwmem_lwmem1'
# 'lwmem'                 'lwmem_lwmem2'
# 'lwmem_alloc_at'        'lwmem_alloc_at_lwmem_alloc_at'
# 'lwmsgq'                'lwmsgq_lwmsgq'
# 'lwsem'                 'lwsem_lwsem1'
# 'lwsem'                 'lwsem_lwsem2'
# 'lwsem'                 'lwsem_lwsem3'
# 'lwtimer'               'lwtimer_lwtimer'
# 'timer'                 'timer_timer'
# 'mem'                   'mem_mem1'
# 'mem'                   'mem_mem2'
# 'mem_alloc_align'       'mem_alloc_align_mem_alloc_align'
# 'mem_alloc_at'          'mem_alloc_at_mem_alloc_at'
# 'mem_copy'              'mem_copy_mem_copy'
# 'mem_zero'              'mem_zero_mem_zero'
# 'message'               'message_message1'
# 'message'               'message_message2'
# 'message'               'message_message3'
# 'mqx'                   'mqx_mqx'
# 'mutex'                 'mutex_mutex1'
# 'mutex'                 'mutex_mutex2'
# 'mutex'                 'mutex_mutex3'
# 'name'                  'name_name'
# 'part'                  'part_part'
# 'preempt'               'preempt_preempt'
# 'queue'                 'queue_queue'
# 'sem'                   'sem_sem'
# 'sched'                 'sched_sched'
# 'task'                  'task_taskat'
# 'task'                  'task_taskq'
# 'task'                  'task_restart'
# 'task'                  'task_abort'
# 'task'                  'task_create'
# 'time'                  'time_time_getset'
# 'tstnset'               'tstnset_tstnset'
# 'usermode'              'usermode_lwevent1'
# 'usermode'              'usermode_lwevent2'
# 'usermode'              'usermode_lwevent3'
# 'usermode'              'usermode_lwmsgq1'
# 'usermode'              'usermode_lwmsgq2'
# 'usermode'              'usermode_lwsem1'
# 'usermode'              'usermode_lwsem2'
# 'usermode'              'usermode_lwsem3'
# 'usermode'              'usermode_lwsem4'
# 'watchdog'              'watchdog_watchdog1'
# 'watchdog'              'watchdog_watchdog2'
# 'yield'                 'yield_yield'
# 'lwgpio'                'lwgpio_tapp_00001'
# 'flashx'                'flashx_flashx'
'mfs'                   'mfs_cache'
'mfs'                   'mfs_date'
'mfs'                   'mfs_delete'
'mfs'                   'mfs_fileswrite'
'mfs'                   'mfs_format'
'mfs'                   'mfs_full'
'mfs'                   'mfs_infile'
'mfs'                   'mfs_install'
'mfs'                   'mfs_rename1'
'mfs'                   'mfs_rename2'
'mfs'                   'mfs_search'
'mfs'                   'mfs_subdirs1'
'mfs'                   'mfs_subdirs2'
)



### ------------------------------------------------------------------
### Build process, just uncomment required type of application
### ------------------------------------------------------------------
let SH2_LOOP1=0
SH2_LAST_MODULE=''
while [ $SH2_LOOP1 -lt ${#SH2_OPTIONS[@]} ]; do

    SH2_DEBGR1=${SH2_OPTIONS[${SH2_LOOP1}]}
    SH2_DEBGR2=${SH2_OPTIONS[${SH2_LOOP1}+1]}
    SH2_DEBGR3=${SH2_OPTIONS[${SH2_LOOP1}+2]}
    SH2_DEBGR4=${SH2_OPTIONS[${SH2_LOOP1}+3]}
    
    export DEBUGGER=$SH2_DEBGR2

    let SH2_LOOP2=0
    while [ $SH2_LOOP2 -lt ${#SH2_MODES[@]} ]; do
        
        SH2_MODE1=${SH2_MODES[${SH2_LOOP2}]}
        SH2_MODE2=${SH2_MODES[${SH2_LOOP2}+1]}
        
        export OUTPUT_DIR=$SH2_OUTPUT_DIR/$SH2_DEBGR1$SH2_MODE2
        
        let SH2_LOOP3=0
        while [ $SH2_LOOP3 -lt ${#SH2_MODULES[@]} ]; do
            
            SH2_MODULE=${SH2_MODULES[${SH2_LOOP3}]}
            SH2_TEST=${SH2_MODULES[${SH2_LOOP3}+1]}
            
        if [ "${SH2_LAST_MODULE}" != "${SH2_MODULE}-${SH2_MODE1}" ]; then
            SH2_LAST_MODULE="${SH2_MODULE}-${SH2_MODE1}"
        ### BSP -----------------------------------------------------------------------------------------------------------------
            make clean_bsp MODULE=$SH2_MODULE BUILD_TYPE=LIB PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2
            
            ccperl $DEV_DIR/_generic_new/build_projects.pl \
            $SH2_DEBGR3 \
            bsp/bsp_$SH2_PLATFORM.in \
            "" \
            $DEV_DIR/lib/$SH2_PLATFORM.$SH2_DEBGR1/bsp/gen \
            $DEV_DIR \
            $SH2_MODE1
            
            make build_bsp -j5 MODULE=$SH2_MODULE BUILD_TYPE=LIB PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2
        ### PSP -----------------------------------------------------------------------------------------------------------------
            make clean_psp MODULE=$SH2_MODULE BUILD_TYPE=LIB PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2
            
            ccperl $DEV_DIR/_generic_new/build_projects.pl \
            $SH2_DEBGR3 \
            psp/psp_$SH2_PLATFORM.in \
            "" \
            $DEV_DIR/lib/$SH2_PLATFORM.$SH2_DEBGR1/psp/gen \
            $DEV_DIR \
            $SH2_MODE1
            
            make build_psp -j5 MODULE=$SH2_MODULE BUILD_TYPE=LIB PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2
        ### MFS -----------------------------------------------------------------------------------------------------------------
            make clean_mfs MODULE=$SH2_MODULE BUILD_TYPE=LIB PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2
            
            ccperl $DEV_DIR/_generic_new/build_projects.pl \
            $SH2_DEBGR3 \
            mfs/mfs_$SH2_PLATFORM.in \
            "" \
            $DEV_DIR/lib/$SH2_PLATFORM.$SH2_DEBGR1/mfs/gen \
            $DEV_DIR \
            $SH2_MODE1
            
            make build_mfs -j5 MODULE=$SH2_MODULE BUILD_TYPE=LIB PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2
        ### ---------------------------------------------------------------------------------------------------------------------
        fi
        ### APP BUILD & EXEC ----------------------------------------------------------------------------------------------------
            make clean TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
            
            ccperl $DEV_DIR/_generic_new/build_projects.pl \
            $SH2_DEBGR3 \
            app/test/test_$SH2_PLATFORM.in \
            $SH2_TEST \
            $DEV_DIR/tests/$SH2_MODULE/make \
            $DEV_DIR \
            $SH2_MODE1
            
            
            make parse  TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
            make build  TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
            make run    TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
            make report TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
        ### APP GUI -------------------------------------------------------------------------------------------------------------
            # make clean TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
            
            # ccperl $DEV_DIR/_generic_new/build_projects.pl \
            # $SH2_DEBGR3 \
            # app/test/test_$SH2_PLATFORM.in \
            # $SH2_TEST \
            # $DEV_DIR/tests/$SH2_MODULE/make \
            # $DEV_DIR \
            # $SH2_MODE1
            
            # perl $DEV_DIR/_generic_new/build_projects.pl \
            # $SH2_DEBGR4 \
            # app/test/test_$SH2_PLATFORM.in \
            # $SH2_TEST \
            # $DEV_DIR/tests/$SH2_MODULE/make \
            # $DEV_DIR \
            # $SH2_MODE1
            
            # make parse  TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
        ### PARSE ---------------------------------------------------------------------------------------------------------------
            # ccperl $DEV_DIR/_generic_new/build_projects.pl \
            # $SH2_DEBGR3 \
            # app/test/test_$SH2_PLATFORM.in \
            # $SH2_TEST \
            # $DEV_DIR/tests/$SH2_MODULE/make \
            # $DEV_DIR \
            # $SH2_MODE1
            
            # make parse  TEST=${SH2_TEST}_${SH2_PLATFORM} MODULE=$SH2_MODULE BUILD_TYPE=APP PLATFORM=$SH2_PLATFORM TOOLCHAIN=$SH2_DEBGR1 CONFIGURATION=$SH2_MODE2 LOAD_TO=$SH2_LOAD_TO
        ### ---------------------------------------------------------------------------------------------------------------------
            let SH2_LOOP3=$SH2_LOOP3+2
        done
        let SH2_LOOP2=$SH2_LOOP2+2
    done
    let SH2_LOOP1=$SH2_LOOP1+4
done




