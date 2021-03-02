/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: system_testing_internal.cpp$
* $Version : 3.8.1.0$
* $Date    : Jul-20-2012$
*
* Comments:
*
*
*
*
*END************************************************************************/

#include "wl_common.h"
#include "nandflash_wl_testing.h"
#include "testing_main_internal.h"
#include "simple_timer.h"
#include "ddi_nand_hal.h"
#include "ddi_media.h"
#include "bad_block_table.h"
#include "phy_map.h"
#include "media_buffer_manager.h"
#include "mfs.h"
#include "system_testing_internal.h"
#include "testing_main_internal.h"
#include "cstring"
#include "mem_management.h"

RtStatus_t system_test_force_repair_scan_bad(MQX_FILE_PTR nand_wl_ptr)
{
    RtStatus_t status  ;

    /* Intialize this test case */
    NandHalSetMqxPtr(0, nand_wl_ptr);

    media_buffer_init();

    status = MediaInit(kInternalNandMedia);
    if (status != SUCCESS) {
        return status;
    }

    status = MediaBuildFromPreDefine();
    if (status != SUCCESS) {
        MediaShutdown(kInternalNandMedia);
        return status;
    }

    nandwl_erase_force_block(nand_wl_ptr, 0, 100);

    nandwl_datadrive_repair_with_scan_bad(nand_wl_ptr);

    /* finalize this test case */
    status = MediaShutdown(kInternalNandMedia);
    if (status != SUCCESS) {
        return status;
    }

    media_buffer_deinit();

    return SUCCESS;

}

RtStatus_t system_test_initialize(bool isRepairWhenInitFailed, MQX_FILE_PTR nand_wl_ptr)
{
    RtStatus_t      status;

    /* Intialize this test case */
    NandHalSetMqxPtr(0, nand_wl_ptr);

    media_buffer_init();

    status = MediaInit(kInternalNandMedia);
    if (status != SUCCESS) {
      return status;
    }

    status = MediaBuildFromPreDefine();
    if (status != SUCCESS) {
      MediaShutdown(kInternalNandMedia);
      return status;
    }

    status = DriveInit(DRIVE_TAG_DATA); // DRIVE_TAG_DATA
    if (status != SUCCESS) {

        if (!isRepairWhenInitFailed)
        {
         MediaShutdown(kInternalNandMedia);
        }
        else /* Try to repair */
        {
            WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR,"=================================================\n");
            WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR,"== Drive Init FAILED, try to REPAIR            ==\n");
            WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR,"=================================================\n");
            _time_delay(2000);
            status = nandwl_datadrive_repair(nand_wl_ptr);
            if (status == ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED)
                status = SUCCESS;
        }
        return status;
    }
    mm_display();

    return SUCCESS;
}
// static RtStatus_t system_test_initialize_drive_failed_with_spy()
RtStatus_t system_test_initialize_drive_failed(MQX_FILE_PTR nand_wl_ptr)
{
   RtStatus_t status  ;
   
   mm_display();
   /* Intialize this test case */
   NandHalSetMqxPtr(0, nand_wl_ptr);
   
   media_buffer_init();
   
   status = MediaInit(kInternalNandMedia);
   if (status != SUCCESS) {   	
      return status;
   } 
   
   status = MediaBuildFromPreDefine();
   if (status != SUCCESS) {
      MediaShutdown(kInternalNandMedia);
      return status;
   }
   
   return ERROR_DDI_LDL_LDRIVE_REPAIR_REQUIRED;
}

RtStatus_t system_test_finalize(MQX_FILE_PTR nand_wl_ptr)
{
    RtStatus_t      status;

    /* finalize this test case */
    status = MediaShutdown(kInternalNandMedia);
    if (status != SUCCESS) {
        return status;
    }

    media_buffer_deinit();

    mm_display();
    mm_display_unfree();
    return SUCCESS;
}

int32_t system_test_mfs_open(MQX_FILE_PTR nand_wl_ptr, char * filesystem_name, MQX_FILE_PTR * filesystem_handle, bool isFormatWhenOpenFailed)
{
    _mqx_int        error_code;

    /* Install MFS over nfc wl driver */
    error_code = _io_mfs_install(nand_wl_ptr, filesystem_name, (_file_size)0);
    if (error_code != MFS_NO_ERROR)
    {
     WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
     return error_code;
    }

    /* Open file system */
    *filesystem_handle = fopen(filesystem_name, NULL);
    error_code = ferror (*filesystem_handle);
    if ((error_code != MFS_NO_ERROR) &&
         ( (error_code != MFS_NOT_A_DOS_DISK) || !isFormatWhenOpenFailed)
         )
    {
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "=================================================\n");
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "Error OPENNING filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "=================================================\n");
        return error_code;
    }
    if ( error_code == MFS_NOT_A_DOS_DISK )
    {
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "=================================================\n");
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "NOT A DOS DISK! You must format to continue.\n");
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "=================================================\n");
        _time_delay(5000);
        error_code = ioctl(*filesystem_handle, IO_IOCTL_DEFAULT_FORMAT, NULL);
        WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, "Format finish with status =%d.\n", error_code);
        /* Even we did format, error still there. Thus, return with err */
        if (error_code != MFS_NO_ERROR)
        {
           fclose(*filesystem_handle);

           return error_code;
        }
    }
    WL_TEST_LOG ("NAND flash installed to %s\n", filesystem_name);
    return MFS_NO_ERROR;
}

int32_t system_test_mfs_close(MQX_FILE_PTR filesystem_handle, char * filesystem_name)
{
    _mqx_int     error_code;

    /* Close the filesystem */
    error_code = fclose (filesystem_handle);
    if (MQX_OK != error_code)
    {
        WL_TEST_LOG("Error closing filesystem.\n");
        return error_code;
    }
    
    /* Uninstall MFS  */
    error_code = _io_dev_uninstall(filesystem_name);
    if (error_code != MFS_NO_ERROR)
    {
        WL_TEST_LOG("Error uninstalling filesystem.\n");
        return error_code;
    }

    return MFS_NO_ERROR;
}

int32_t system_test_read_write_with_mfs_write_internal
(
    MQX_FILE_PTR    nand_wl_ptr,
    char *        fsname, 
    unsigned char *       * write_buffer_arr,
    int32_t *      write_buffer_size,
    uint32_t         num_files_write 
)
{
   char fpath[PATHNAME_SIZE] = {0};
   MQX_FILE_PTR fshandle = NULL;
   MQX_FILE_PTR fhandle = NULL;
   uint32_t i ;
   int32_t ret = -1;


   if (SUCCESS != system_test_initialize(TRUE, nand_wl_ptr))
   goto err_handle;


   if (MFS_NO_ERROR != system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE))
   {
      goto err_handle;
   }

   // TODO: confirm that this block of secnum is not bad

   for (i=0; i< num_files_write; i++)
   {
      sprintf(fpath, "%s\\FILE%d", fsname, i);
      WL_TEST_LOG("Write data on file %s \n",&fpath);

      fhandle = fopen(fpath, "w");
      if (!fhandle) goto err_handle;

      fseek(fhandle, 0, IO_SEEK_SET );
      WL_TEST_LOG("Write to file %d by using buffer 0x%0x with size %d\n",
      i,(unsigned char *)(write_buffer_arr[i]), write_buffer_size[i] );

      if (write_buffer_size[i] != write(fhandle, (unsigned char *)(write_buffer_arr[i]), write_buffer_size[i]  ) )
      {
         WL_TEST_LOG("Write file %s failed \n", fpath);
         goto err_handle;
      }

      /*dump_buff((unsigned char *)(write_buffer_arr[i]), 512, fpath);*/
      fclose(fhandle);
      fhandle = NULL;
   }

   ret = MQX_OK;
   err_handle:


   if (NULL != fhandle)
   {
      fclose(fhandle);
   }

   if (NULL != fshandle )
   {
      system_test_mfs_close(fshandle, fsname);
   }
   system_test_finalize(nand_wl_ptr);
   return ret;
}

int32_t system_test_read_write_with_mfs_read_internal
(
    MQX_FILE_PTR    nand_wl_ptr,
    char *        fsname, 
    unsigned char *       * read_buffer_arr, 
    int32_t *      read_buffer_size, 
    uint32_t         num_files_write
)
{
   char fpath[PATHNAME_SIZE] = {0};
   MQX_FILE_PTR fshandle = NULL;
   MQX_FILE_PTR fhandle = NULL;
   uint32_t i = 0;
   int32_t ret = -1;


   if (SUCCESS != system_test_initialize(TRUE, nand_wl_ptr))
   goto err_handle;

   if (MFS_NO_ERROR != system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, FALSE))
   {
      goto err_handle;
   }

   // TODO: confirm that this block of secnum is not bad

   for (i=0; i< num_files_write; i++)
   {
      sprintf(fpath, "%s\\FILE%d", fsname, i);
      WL_TEST_LOG("Read data on file %s \n", &fpath);

      fhandle = fopen(fpath, "r");
      if (!fhandle) goto err_handle;

      fseek(fhandle, 0, IO_SEEK_SET );
      WL_TEST_LOG("Read in file %d by using buffer 0x%0x with size %d\n",
      i,(unsigned char *)(read_buffer_arr[i]), read_buffer_size[i] );
      if (read_buffer_size[i] != read(fhandle, (unsigned char *)(read_buffer_arr[i]), read_buffer_size[i]  ) )
      {
         WL_TEST_LOG("Read file %s failed \n", fpath);
         goto err_handle;
      }
      /*dump_buff((unsigned char *)(read_buffer_arr[i]), 512, fpath);*/


      fclose(fhandle);
      fhandle = NULL;
   }

   ret = MQX_OK;

   err_handle:


   if (NULL != fhandle)
   {
      fclose(fhandle);
   }

   if (NULL != fshandle )
   {
      system_test_mfs_close(fshandle, fsname);
   }
   system_test_finalize(nand_wl_ptr);
   return ret;
}

int32_t system_test_read_write_with_mfs_mkdir_internal(
        MQX_FILE_PTR nand_wl_ptr, char * fsname, dir_arrs_type_ptr dirs_arr, uint32_t num_dir, bool isNested)
{
   char fpath[PATHNAME_SIZE*2] = {0};
   MQX_FILE_PTR fshandle = NULL;
   MQX_FILE_PTR fhandle = NULL;
   uint32_t i = 0;
   int32_t ret = -1, error;


   if (SUCCESS != system_test_initialize(TRUE, nand_wl_ptr))
   goto err_handle;

   if (MFS_NO_ERROR != system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE))
   {
      goto err_handle;
   }

   error = ioctl(fshandle, IO_IOCTL_DEFAULT_FORMAT, NULL);
   if (error)
   {
      WL_TEST_LOG("Format error %s\n", MFS_Error_text(error) );
      goto err_handle;
   }
   // TODO: confirm that this block of secnum is not bad

   i = 0;

   /* Move to root directory X:\ */
   sprintf(fpath, "%s\\", fsname );
   error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);

   if (error) goto err_handle;

   while (i < num_dir)
   {
      WL_TEST_LOG("Create dir %s \n", (*dirs_arr)[i]);

      error = ioctl(fshandle, IO_IOCTL_CREATE_SUBDIR, (void *) (*dirs_arr)[i]);
      if (error)
      {

         WL_TEST_LOG("IO_IOCTL_CREATE_SUBDIR error %s\n", MFS_Error_text(error) );

         goto err_handle;
      }

      if (isNested)
      {
         /* Move to recent created directory */
         error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR,  (void *)(*dirs_arr)[i]);
         if (error) goto err_handle;
      }
      i++;
   }

   ret = MQX_OK;

   err_handle:

   if (NULL != fshandle )
   {
      system_test_mfs_close(fshandle, fsname);
   }
   system_test_finalize(nand_wl_ptr);
   return ret;
}

int32_t system_test_read_write_with_mfs_readdir_internal
(
    MQX_FILE_PTR        nand_wl_ptr, 
    char *            fsname, 
    dir_arrs_type_ptr   dirs_arr,
    uint32_t             num_dir, 
    bool             isNested
)
{
   char fpath[PATHNAME_SIZE] = {0};
   MQX_FILE_PTR fshandle = NULL;
   MQX_FILE_PTR fhandle = NULL;
   uint32_t i = 0;
   int32_t ret = -1, error;


   if (SUCCESS != system_test_initialize(TRUE, nand_wl_ptr))
   goto err_handle;

   if (MFS_NO_ERROR != system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, FALSE))
   {
      goto err_handle;
   }

   sprintf(fpath, "%s\\", fsname); /* Root dir */
   error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);

   if (isNested)
   {

      DIR_STRUCT_PTR dir_ptr;
      char dir_name[MAX_DIR_NAME_LENGTH];
      char dir_path[PATHNAME_SIZE] =   { 0   };
      _mqx_int error_code;

      /*  Move to root directory */
      sprintf((char *)&dir_path, "%s\\", fsname );
      error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, (char *)&dir_path);
      if (error) goto err_handle;

      sprintf((char *)&dir_path, "*.*"/*, fsname */);
      i = 0;
      while (i < num_dir)
      {

         dir_ptr =(DIR_STRUCT_PTR) _io_mfs_dir_open(fshandle, (char *)&dir_path, "d" ); /*MFS_ATTR_DIR_NAME*/
         do {

            if ( dir_ptr->FIRST )
            {
               error_code = ioctl(fshandle, IO_IOCTL_FIND_FIRST_FILE, (void *)&dir_ptr->SEARCH);
               dir_ptr->FIRST = FALSE;
            }
            else
            {
               error_code = ioctl(fshandle, IO_IOCTL_FIND_NEXT_FILE,  (void *) &dir_ptr->SEARCH_DATA);
            }
            if ( error_code != MFS_NO_ERROR )
            {
               break;
            }
         }
         while ( ((dir_ptr->SEARCH_DATA.NAME[0] == '.' )
         && (strlen(dir_ptr->SEARCH_DATA.NAME) ==1 ) )
         || ( (dir_ptr->SEARCH_DATA.NAME[0] == '.' ) && (dir_ptr->SEARCH_DATA.NAME[1] == '.' )
         && (strlen(dir_ptr->SEARCH_DATA.NAME) ==2 ) ));

         error_code = ioctl(fshandle, IO_IOCTL_GET_LFN, (void *)&dir_ptr->LFN_DATA);

         if ( !error_code )
         {
            strcpy( (char *)((*dirs_arr)[i]) , (char *)&dir_ptr->LFN);
            (i)++;
            error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, (char *)&dir_ptr->LFN);
            if (error) goto err_handle;
         }
         else
         {
            strcpy( (char *)((*dirs_arr)[i]) , (char *)&dir_ptr->SEARCH_DATA.NAME);
            (i)++;
            error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, (char *)&dir_ptr->SEARCH_DATA.NAME);
            if (error) goto err_handle;
         }
         _io_mfs_dir_close(dir_ptr);

      }

   }
   else
   {
      DIR_STRUCT_PTR dir_ptr;
      char dir_name[MAX_DIR_NAME_LENGTH];
      char dir_path[PATHNAME_SIZE] =   { 0   };
      _mqx_int error_code;

      /*  Move to root directory */
      sprintf((char *)&dir_path, "%s\\", fsname );
      error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, (char *)&dir_path);
      if (error) goto err_handle;

      sprintf((char *)&dir_path, "%s\\*.*", fsname);
      dir_ptr =(DIR_STRUCT_PTR) _io_mfs_dir_open(fshandle, (char *)&dir_path, "d" ); /*MFS_ATTR_DIR_NAME*/

      i = 0;
      while (i < num_dir)
      {

         if ( dir_ptr->FIRST )
         {
            error_code = ioctl(fshandle, IO_IOCTL_FIND_FIRST_FILE, (void *)&dir_ptr->SEARCH);
            dir_ptr->FIRST = FALSE;
         }
         else
         {
            error_code = ioctl(fshandle, IO_IOCTL_FIND_NEXT_FILE,  (void *) &dir_ptr->SEARCH_DATA);
         }
         if ( error_code != MFS_NO_ERROR )
         {
            break;
         }

         error_code = ioctl(fshandle, IO_IOCTL_GET_LFN, (void *)&dir_ptr->LFN_DATA);

         if ( !error_code )
         {
            strcpy( (char *)((*dirs_arr)[i]) , (char *)&dir_ptr->LFN);
            (i)++;
         }
         else
         {
            strcpy( (char *)((*dirs_arr)[i]) , (char *)&dir_ptr->SEARCH_DATA.NAME);
            (i)++;
         }
      }

      _io_mfs_dir_close(dir_ptr);
   }

   ret = MQX_OK;

   err_handle:

   if (error)
   {
      WL_TEST_LOG("Readdir error %s\n", MFS_Error_text(error) );
      goto err_handle;
   }

   if (NULL != fshandle )
   {
      system_test_mfs_close(fshandle, fsname);
   }
   system_test_finalize(nand_wl_ptr);
   return ret;
}

int32_t system_test_read_write_with_mfs_write
(
    MQX_FILE_PTR nand_wl_ptr,
    char * fsname,
    unsigned char * write_buffer,
    int32_t write_buffer_size,
    uint32_t num_files_write
)
{
   char             fpath[PATHNAME_SIZE] = {0};
   MQX_FILE_PTR     fshandle = NULL;
   MQX_FILE_PTR     fhandle = NULL;
   uint32_t          i ;
   int32_t           ret = -1;

   if (SUCCESS != system_test_initialize(TRUE, nand_wl_ptr))
   goto err_handle;

   if (MFS_NO_ERROR != system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE))
   {
      goto err_handle;
   }
   // TODO: confirm that this block of secnum is not bad
   for (i=0; i< num_files_write; i++)
   {
      sprintf(fpath, "%s\\FILE%d", fsname, i);
      fhandle = fopen(fpath, "w");
      if (!fhandle) goto err_handle;
      fseek(fhandle, 0, IO_SEEK_SET );
      if (write_buffer_size != write(fhandle, (unsigned char *)(write_buffer), write_buffer_size))
      {
         WL_TEST_LOG("Write file %s failed \n", fpath);
         goto err_handle;
      }
      fclose(fhandle);
      fhandle = NULL;
   }

   ret = MQX_OK;
   err_handle:


   if (NULL != fhandle)
   {
      fclose(fhandle);
   }

   if (NULL != fshandle )
   {
      system_test_mfs_close(fshandle, fsname);
   }
   system_test_finalize(nand_wl_ptr);
   return ret;
}

int32_t system_test_read_write_with_mfs_read
(
    MQX_FILE_PTR nand_wl_ptr,
    char * fsname,
    unsigned char * read_buffer,
    int32_t read_buffer_size,
    uint32_t num_files_write
)
{
   char fpath[PATHNAME_SIZE] = {0};
   MQX_FILE_PTR fshandle = NULL;
   MQX_FILE_PTR fhandle = NULL;
   uint32_t i = 0;
   int32_t ret = -1;


   if (SUCCESS != system_test_initialize(TRUE, nand_wl_ptr))
   goto err_handle;

   if (MFS_NO_ERROR != system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, FALSE))
   {
      goto err_handle;
   }

   // TODO: confirm that this block of secnum is not bad
   for (i=0; i< num_files_write; i++)
   {
      sprintf(fpath, "%s\\FILE%d", fsname, i);

      fhandle = fopen(fpath, "r");
      if (!fhandle) goto err_handle;
      fseek(fhandle, 0, IO_SEEK_SET );
      if (read_buffer_size != read(fhandle, (unsigned char *)(read_buffer), read_buffer_size))
      {
         WL_TEST_LOG("Read file %s failed \n", fpath);
         goto err_handle;
      }
      fclose(fhandle);
      fhandle = NULL;
   }
   ret = MQX_OK;
   err_handle:
   if (NULL != fhandle)
   {
      fclose(fhandle);
   }
   if (NULL != fshandle )
   {
      system_test_mfs_close(fshandle, fsname);
   }
   system_test_finalize(nand_wl_ptr);
   return ret;
}

/* EOF */
