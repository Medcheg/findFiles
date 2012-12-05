#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define _STRLEN_ 1024

int TotalSize;
int TotalFiles;
int LinesCount = 0;
int FileCounter = 0;
FILE *h_log;


char *check_file_extension(const char *in_str, const char *extension)
{
    int len1 = strlen(in_str);
    int len2 = strlen(extension);

    return strstr(&in_str[len1-len2], extension);
}

// --------------------------
void check_phrase_in_file(const char *filepath, const char *filename, int file_size, time_t *m_time)
{
    //int i;
    char *copyright_str = "Copyright";
    char buff [_STRLEN_];
    char fname[_STRLEN_];
    FILE *h_file;
    
    if ( strstr(filepath, "\\.")    != NULL ) return;
    if ( strstr(filepath, "u-boot") != NULL ) return;
    if ( strstr(filepath, "doc")    != NULL ) return;

    if ( check_file_extension(filename, ".so" ) != NULL ) return;
    if ( check_file_extension(filename, ".o"  ) != NULL ) return;
    if ( check_file_extension(filename, ".pdf") != NULL ) return;
    if ( check_file_extension(filename, ".png") != NULL ) return;
    if ( check_file_extension(filename, ".tar") != NULL ) return;
    if ( filename[0] == '.') return;

    sprintf(fname, "%s/%s", filepath, filename);

    h_file = fopen(fname, "r");

    if ( h_file == NULL )
    {
        return;
    }

    // count bytte for reading
    int count_bytes_for_reading = ( file_size > _STRLEN_ ) ? _STRLEN_ : file_size;

    if ( fread(buff, 1, count_bytes_for_reading, h_file) != 0 )
    {
        //try to find copyright string
        char *ptr = strstr(buff, copyright_str);

        char *pchtime = ctime(m_time);
        pchtime[strlen(pchtime) - 1] = 0;

        if ( ptr == NULL )
        {
            fprintf(h_log, "%s\n", fname);
            printf("%s\t%i\t%s\n", pchtime, file_size, fname);
        }
    }
    fclose(h_file);
}

// --------------------------
void Calculate_CountLines(const char *filepath, const char *filename, int file_size, time_t *m_time)
{
    int i, local_lines_count;
    char *buff = (char*) malloc (file_size);
    char fname[_STRLEN_];
    FILE *h_file;


    // -------------
//    if ( strstr(filepath, "thirdparty") != NULL ) return;
//    if ( strstr(filepath, "tests") != NULL ) return;
//    if ( strstr(filepath, ".git") != NULL ) return;
//    if ( strstr(filepath, "u-boot") != NULL ) return;
//    if ( strstr(filepath, "/usr/sbin/debug") != NULL ) return;
//    if ( strstr(filepath, "/usr/sbin/diskutil") != NULL ) return;
//    if ( strstr(filepath, "/usr/sbin/lock") != NULL ) return;
//    if ( strstr(filepath, "/usr/sbin/ktrace") != NULL ) return;
//    if ( strstr(filepath, "/usr/bin/ktrace") != NULL ) return;
//    if ( strstr(filepath, "thirdparty") != NULL ) return;
//    if ( strstr(filepath, "tests") != NULL ) return;
//    if ( strstr(filepath, "/tzsw/include/interface") != NULL ) return;
//    if ( strstr(filepath, "/src/tztest") != NULL ) return;
/*
      if ( strstr(filepath, "/tzsw/") == NULL ) return;
      if ( strstr(filepath, "/tzsw/") == NULL ) return;
      if ( strstr(filepath, "tz_ta_hdcp") != NULL ) return;
      if ( strstr(filepath, "tzapi_test_app") != NULL ) return;
      if ( strstr(filepath, "tz_ta_sstorage") != NULL ) return;
*/      
    // -------------
    if ( check_file_extension(filename, ".s") == NULL )
        if ( check_file_extension(filename, ".S") == NULL )
            if ( check_file_extension(filename, ".cpp") == NULL )
                if ( check_file_extension(filename, ".h") == NULL )
                    if ( check_file_extension(filename, ".c") == NULL ) return;

    sprintf(fname, "%s/%s", filepath, filename);

    h_file = fopen(fname, "r");

    if ( h_file == NULL )
    {
        return;
    }

    if ( fread(buff, 1, file_size, h_file) != 0 )
    {
        local_lines_count = 0;
        for ( i = 0; i < file_size; i++)
            if (buff[i] == '\n') local_lines_count++;

        LinesCount += local_lines_count;
        FileCounter++;

        //fprintf(h_log, "%i\t%i\t%i\t%s\n",FileCounter, local_lines_count, LinesCount, fname);
        printf("%i\t%i\t%i\t%s\n", FileCounter, local_lines_count, LinesCount, fname);
    }
    fclose(h_file);
    free(buff);
}

    
//-----------------------------------------------
int find (const char *startDir, const char *fileMask)
{
   DIR *pDir;                                
   struct dirent *pDirent;                     
   
   if ((pDir= opendir (startDir)) == NULL)
   {
      printf ("%s: No such file or directory.\n", startDir);
      return -1;
   }

   while ((pDirent = readdir (pDir)) != NULL)    
   {
      if (!strcmp (pDirent->d_name, ".") || !strcmp (pDirent->d_name, "..")) 
         continue;
      
      char bufStr[_STRLEN_];             
      
      strcpy (bufStr, startDir);
      strcat (bufStr, "/");              
      strcat (bufStr, pDirent->d_name);  
      
      struct stat bufInfo;                      
      if ( stat (bufStr, &bufInfo) == -1 )
      {
            // error: pstat
          //return errno;
      }
      
      if (S_ISDIR (bufInfo.st_mode))
      {
          printf("%s\t%s\n", pDirent->d_name, startDir);
          find (bufStr, fileMask);
      }

        if (S_ISREG (bufInfo.st_mode) )
           printf("---->  %s\n", pDirent->d_name);

      //if (S_ISREG (bufInfo.st_mode) )
      //if ( check_file_extension(pDirent->d_name, fileMask) == NULL )
       //       continue;
         
      //if (S_ISREG (bufInfo.st_mode))
            //check_phrase_in_file(startDir, pDirent->d_name, bufInfo.st_size, &bufInfo.st_mtime);
        //    Calculate_CountLines(startDir, pDirent->d_name, bufInfo.st_size, &bufInfo.st_mtime);
    }

   closedir (pDir);
   
   return 0;
}

//-----------------------------------------------
int main (int argc, char **argv)
{
    h_log = fopen("log.txt", "w");

    printf("last modification\t\tsize\tname\n");
    printf("-------------------------------------------\n");

    find ("/home/pm/workspace/.app/source/linux/find_files", ".h");
   
    fclose(h_log);
   
   return 0;
}
