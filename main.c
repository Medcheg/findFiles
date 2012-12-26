/**
 * \par Copyright: (c)
**/

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define _STRLEN_ 1024



int TotalSize;
int TotalFiles;
int LinesCount = 0;
int FileCounter = 0;
FILE *h_log;
//

//
typedef struct tag_file
{
    char  *name;     // file name
    int    size;     // file size
    time_t mtime;   // time of last modification
}file_st;

file_st *FileList = NULL;
int     FileList_len = 0;

//-----------------------------------------------
int check_file_mask(const char *in_str, const char *extension)
{
    int len1 = strlen(in_str);
    int len2 = 0;
    char ext[32] = {0};
    const char *ch_beginpos = extension;
    const char *ch_currpos =  extension;

    if ( len1 > 2048 )
        return 0; // something wrong with input string

    if ( strstr(extension, ".*") != NULL )
        return 1; // SUCCESS !!!


    //printf(" -----> check_file_mask ------>");
    while ( 1 )
    {
        if ( *ch_currpos == ' ' || *ch_currpos == '\0' )
        {
            len2 = ch_currpos-ch_beginpos;
            strncpy(ext, ch_beginpos, len2);
            ext[len2] = '\0';

            if ( NULL != strstr(&in_str[len1 - len2], ext) )
            {
                //printf(" [  OK  ]\t%s\n", in_str);
                return 1; // SUCCESS !!!
            }

            if ( *ch_currpos == '\0' )
            {
                //printf(" [  OK  ]\t%s\n", in_str);
                return 0; // not found
            }

            //printf("exxt = %s\t file = %s\n", ext, in_str);
            ch_beginpos = ch_currpos + 1;
        }

        ch_currpos++;
    }
    //printf(" [  fail ]\t%s\n", in_str);

    return 0; // not found
}

//-----------------------------------------------
int check_file_path(const char *in_str, const char *extension)
{
    int len1 = strlen(in_str);
    int len2 = 0;
    char ext[32] = {0};
    const char *ch_beginpos = extension;
    const char *ch_currpos =  extension;

    if ( len1 > 2048 )
        return 0; // something wrong with input string

    if ( strstr(extension, ".*") != NULL )
        return 1; // SUCCESS !!!

    while ( 1 )
    {
        if ( *ch_currpos == ' ' || *ch_currpos == '\0' )
        {
            len2 = ch_currpos-ch_beginpos;
            strncpy(ext, ch_beginpos, len2);
            ext[len2] = '\0';

            if ( NULL != strstr(in_str, ext) )
                return 1; // SUCCESS !!!

            if ( *ch_currpos == '\0' )
                return 0; // not found

            ch_beginpos = ch_currpos + 1;
        }

        ch_currpos++;
    }
    return 0; // not found
}

//
int find (const char *startDir, const char *search_fileMask, const char *ignore_fileMask, const char *ignore_path)
{
    DIR *pDir;
    struct dirent *pDirent;

    if ((pDir= opendir (startDir)) == NULL)
    {
        printf ("%s: No such file or directory.\n", startDir);
        return errno;
    }

    while ((pDirent = readdir (pDir)) != NULL)
    {
        if (!strcmp (pDirent->d_name, ".") || !strcmp (pDirent->d_name, ".."))
            continue;

        char fullName[_STRLEN_];
        struct stat bufInfo;

        sprintf(fullName, "%s/%s", startDir, pDirent->d_name);
        if ( stat (fullName, &bufInfo) == -1 )
            return errno;

        if (S_ISDIR (bufInfo.st_mode))
            find (fullName, search_fileMask, ignore_fileMask, ignore_path);

        if ( S_ISREG (bufInfo.st_mode) == 0 )
            continue;

        // check file path
        if ( check_file_path(startDir, ignore_path) != 0 )
            continue;

        // check file mask
        if ( check_file_mask(pDirent->d_name, search_fileMask) == 0 )
            continue;

        // check file mask
        if ( check_file_mask(pDirent->d_name, ignore_fileMask) != 0 )
        {
            //printf("-----> [ fail ]\t%s\n", pDirent->d_name);
            continue;
        }
        //printf("-----> [  OK  ]\t%s\n", pDirent->d_name);
        if ( strcmp(pDirent->d_name, "tz_ass.files") == 0 )
            exit(1);

        //printf("%s ---->  %s\n", startDir, pDirent->d_name);

        if ( FileList_len == 0 )
            FileList = (file_st*) malloc (sizeof(file_st));
        else
            FileList = (file_st*) realloc( FileList, sizeof(file_st)*(FileList_len+1) );

        FileList[FileList_len].name  = strdup(fullName);
        FileList[FileList_len].size  = bufInfo.st_size;
        FileList[FileList_len].mtime = bufInfo.st_mtime;
        FileList_len++;
    }

   closedir (pDir);

   return errno;
}

// --------------------------
void check_phrase_in_file(char *phrase, file_st *ffile)
{
    //int i;
    char buff [_STRLEN_];
    FILE *h_file;
    
    h_file = fopen(ffile->name, "r");

    if ( h_file == NULL )
    {
        return;
    }

    // count byte for reading
    int count_bytes_for_reading = ( ffile->size > _STRLEN_ ) ? _STRLEN_ : ffile->size;

    if ( fread(buff, 1, count_bytes_for_reading, h_file) != 0 )
    {
        //try to find copyright string
        char *ptr = strstr(buff, phrase);

        char *pchtime = ctime(&ffile->mtime);
        pchtime[strlen(pchtime) - 1] = 0;

        if ( ptr == NULL )
        {
            fprintf(h_log, "%s\n", ffile->name);
            printf("%s\t%d\t%s\n", pchtime, ffile->size, ffile->name);
        }
    }
    fclose(h_file);
}

// --------------------------
//void Calculate_CountLines(const char *filepath, const char *filename, int file_size, time_t *m_time)
//{
//    int i, local_lines_count;
//    char *buff = (char*) malloc (file_size);
//    char fname[_STRLEN_];
//    FILE *h_file;

//    sprintf(fname, "%s/%s", filepath, filename);

//    h_file = fopen(fname, "r");

//    if ( h_file == NULL )
//    {
//        return;
//    }

//    if ( fread(buff, 1, file_size, h_file) != 0 )
//    {
//        local_lines_count = 0;
//        for ( i = 0; i < file_size; i++)
//            if (buff[i] == '\n') local_lines_count++;

//        LinesCount += local_lines_count;
//        FileCounter++;

//        //fprintf(h_log, "%i\t%i\t%i\t%s\n",FileCounter, local_lines_count, LinesCount, fname);
//        printf("%i\t%i\t%i\t%s\n", FileCounter, local_lines_count, LinesCount, fname);
//    }
//    fclose(h_file);
//    free(buff);
//}


//-----------------------------------------------
void fileList_delete()
{
    int i;

    if ( FileList != NULL )
    {
        for ( i = 0; i < FileList_len; i++ )
        {
            if ( FileList[i].name != NULL )
                 free(FileList[i].name);
        }
        free(FileList);
        FileList_len = 0;
    }
}

//-----------------------------------------------
int main (int argc, char **argv)
{
    int i;

    h_log = fopen("log.txt", "w+");

    char *path        = ".";
    char *search_mask = ".*";
    char *ignore_mask = ".user .creator .files .includes .doc .ko .config .o .d";
    char *ignore_path = ".git libs bin";
    char *checkPhrase = "Copyright";

    printf(" 1. ---> Search Files\n");
    printf("         Ignore file mask = '%s'\n", ignore_mask);
    printf("         Ignore file path = '%s'\n", ignore_path);
    if ( find (path, search_mask, ignore_mask, ignore_path) != 0 )
        printf ("Error: %d\n\n", errno);

    printf(" 2. ---> Check phrase - '%s'\n", checkPhrase);
    printf("\n");
    printf("last modification\t\tsize\tname\n\n");

    for ( i = 0; i < FileList_len; i++ )
        check_phrase_in_file(checkPhrase, &FileList[i]);

    fileList_delete();

    fclose(h_log);

   return 0;
}
