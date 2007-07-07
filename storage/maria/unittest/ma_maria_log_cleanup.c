#include "../maria_def.h"
#include <my_dir.h>

my_bool maria_log_remove()
{
  MY_DIR *dirp;
  uint i;
  MY_STAT stat_buff;
  char file_name[FN_REFLEN];

  /* Removes control file */
  if (fn_format(file_name, CONTROL_FILE_BASE_NAME,
                maria_data_root, "", MYF(MY_WME)) == NullS)
    return 1;
  if (my_stat(file_name, &stat_buff, MYF(0)) &&
      my_delete(file_name, MYF(MY_WME)) != 0)
    return 1;

  /* Finds and removes transaction log files */
  if (!(dirp = my_dir(maria_data_root, MYF(MY_DONT_SORT))))
    return 1;

  for (i= 0; i < dirp->number_off_files; i++)
  {
    char *file= dirp->dir_entry[i].name;
    if (strncmp(file, "maria_log.", 10) == 0 &&
        file[10] >= '0' && file[10] <= '9' &&
        file[11] >= '0' && file[11] <= '9' &&
        file[12] >= '0' && file[12] <= '9' &&
        file[13] >= '0' && file[13] <= '9' &&
        file[14] >= '0' && file[14] <= '9' &&
        file[15] >= '0' && file[15] <= '9' &&
        file[16] >= '0' && file[16] <= '9' &&
        file[17] >= '0' && file[17] <= '9' &&
        file[18] == '\0')
    {
      if (fn_format(file_name, file,
                    maria_data_root, "", MYF(MY_WME)) == NullS ||
          my_delete(file_name, MYF(MY_WME)) != 0)
        return 1;
    }
  }
  return 0;
}

