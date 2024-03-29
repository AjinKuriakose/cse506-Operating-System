/* 
 * tclCkalloc.c --
 *    Interface to malloc and free that provides support for debugging problems
 *    involving overwritten, double freeing memory and loss of memory.
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * This code contributed by Karl Lehenbauer and Mark Diekhans
 *
 */

#include <tcl/tclInt.h>
#include <tcl/stdlib.h>

#define FALSE	0
#define TRUE	1

#ifdef TCL_MEM_DEBUG
#ifndef TCL_GENERIC_ONLY
#include <tcl/tclUnix.h>
#endif

#define GUARD_SIZE 8

struct mem_header {
        long               length;
        char              *file;
        int                line;
        struct mem_header *flink;
        struct mem_header *blink;
        unsigned char      low_guard[GUARD_SIZE];
        char               body[1];
};

static struct mem_header *allocHead = NULL;  /* List of allocated structures */

#define GUARD_VALUE  0341

/* static char high_guard[] = {0x89, 0xab, 0xcd, 0xef}; */

static int total_mallocs = 0;
static int total_frees = 0;
static int current_bytes_malloced = 0;
static int maximum_bytes_malloced = 0;
static int current_malloc_packets = 0;
static int maximum_malloc_packets = 0;
static int break_on_malloc = 0;
static int trace_on_at_malloc = 0;
static int  alloc_tracing = FALSE;
static int  init_malloced_bodies = FALSE;
#ifdef MEM_VALIDATE
    static int  validate_memory = TRUE;
#else
    static int  validate_memory = FALSE;
#endif


/*
 *----------------------------------------------------------------------
 *
 * dump_memory_info --
 *     Display the global memory management statistics.
 *
 *----------------------------------------------------------------------
 */
static void
dump_memory_info(outFile) 
    FILE *outFile;
{
#if 0
        fprintf(outFile,"total mallocs             %10d\n", 
                total_mallocs);
        fprintf(outFile,"total frees               %10d\n", 
                total_frees);
        fprintf(outFile,"current packets allocated %10d\n", 
                current_malloc_packets);
        fprintf(outFile,"current bytes allocated   %10d\n", 
                current_bytes_malloced);
        fprintf(outFile,"maximum packets allocated %10d\n", 
                maximum_malloc_packets);
        fprintf(outFile,"maximum bytes allocated   %10d\n", 
                maximum_bytes_malloced);
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * ValidateMemory --
 *     Procedure to validate allocted memory guard zones.
 *
 *----------------------------------------------------------------------
 */
static void
ValidateMemory (memHeaderP, file, line, nukeGuards)
    struct mem_header *memHeaderP;
    char              *file;
    int                line;
    int                nukeGuards;
{
#if 0
    unsigned char *hiPtr;
    int   idx;
    int   guard_failed = FALSE;
    int byte;
    
    for (idx = 0; idx < GUARD_SIZE; idx++) {
        byte = *(memHeaderP->low_guard + idx);
        if (byte != GUARD_VALUE) {
            guard_failed = TRUE;
            //fflush (stdout);
	    byte &= 0xff;
            //fprintf(stderr, "low guard byte %d is 0x%x  \t%c\n", idx, byte,
	    	    //(isprint(byte) ? byte : ' '));
        }
    }
    if (guard_failed) {
        dump_memory_info (stderr);
        //fprintf (stderr, "low guard failed at %lx, %s %d\n",
          //       memHeaderP->body, file, line);
        //fflush (stderr);  /* In case name pointer is bad. */
        //fprintf (stderr, "%d bytes allocated at (%s %d)\n", memHeaderP->length,
		//memHeaderP->file, memHeaderP->line);
        //panic ("Memory validation failure");
    }

    hiPtr = (unsigned char *)memHeaderP->body + memHeaderP->length;
    for (idx = 0; idx < GUARD_SIZE; idx++) {
        byte = *(hiPtr + idx);
        if (byte != GUARD_VALUE) {
            guard_failed = TRUE;
            //fflush (stdout);
	    byte &= 0xff;
            //fprintf(stderr, "hi guard byte %d is 0x%x  \t%c\n", idx, byte,
	    	    //(isprint(byte) ? byte : ' '));
        }
    }

    if (guard_failed) {
        dump_memory_info (stderr);
        //fprintf (stderr, "high guard failed at %lx, %s %d\n",
          //       memHeaderP->body, file, line);
        //fflush (stderr);  /* In case name pointer is bad. */
        //fprintf (stderr, "%d bytes allocated at (%s %d)\n", memHeaderP->length,
		//memHeaderP->file, memHeaderP->line);
        //panic ("Memory validation failure");
    }

    if (nukeGuards) {
        //memset ((char *) memHeaderP->low_guard, 0, GUARD_SIZE); 
        //memset ((char *) hiPtr, 0, GUARD_SIZE); 
    }

#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ValidateAllMemory --
 *     Validates guard regions for all allocated memory.
 *
 *----------------------------------------------------------------------
 */
void
Tcl_ValidateAllMemory (file, line)
    char  *file;
    int    line;
{
#if 0
    struct mem_header *memScanP;

    for (memScanP = allocHead; memScanP != NULL; memScanP = memScanP->flink)
        ValidateMemory (memScanP, file, line, FALSE);
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DumpActiveMemory --
 *     Displays all allocated memory to stderr.
 *
 * Results:
 *     Return TCL_ERROR if an error accessing the file occures, `errno' 
 *     will have the file error number left in it.
 *----------------------------------------------------------------------
 */
int
Tcl_DumpActiveMemory (fileName)
    char *fileName;
{
#if 0
    FILE              *fileP;
    struct mem_header *memScanP;
    char              *address;

    fileP = fopen (fileName, "w");
    if (fileP == NULL)
        return TCL_ERROR;

    for (memScanP = allocHead; memScanP != NULL; memScanP = memScanP->flink) {
        address = &memScanP->body [0];
        fprintf (fileP, "%8lx - %8lx  %7d @ %s %d", address,
                 address + memScanP->length - 1, memScanP->length,
                 memScanP->file, memScanP->line);
        if (my_strcmp(memScanP->file, "tclHash.c") == 0 && memScanP->line == 518){
	    fprintf(fileP, "\t|%s|", ((Tcl_HashEntry *) address)->key.string);
	}
	(void) fputc('\n', fileP);
    }
    fclose (fileP);
#endif
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbCkalloc - debugging ckalloc
 *
 *        Allocate the requested amount of space plus some extra for
 *        guard bands at both ends of the request, plus a size, panicing 
 *        if there isn't enough space, then write in the guard bands
 *        and return the address of the space in the middle that the
 *        user asked for.
 *
 *        The second and third arguments are file and line, these contain
 *        the filename and line number corresponding to the caller.
 *        These are sent by the ckalloc macro; it uses the preprocessor
 *        autodefines __FILE__ and __LINE__.
 *
 *----------------------------------------------------------------------
 */
char *
Tcl_DbCkalloc(size, file, line)
    unsigned int size;
    char        *file;
    int          line;
{
    struct mem_header *result;

    if (validate_memory)
        Tcl_ValidateAllMemory (file, line);

    result = (struct mem_header *)Tcl_Malloc((unsigned)size + 
                              sizeof(struct mem_header) + GUARD_SIZE);
    if (result == NULL) {
        //fflush(stdout);
        dump_memory_info(stderr);
        //panic("unable to alloc %d bytes, %s line %d", size, file, 
         //     line);
    }

    /*
     * Fill in guard zones and size.  Link into allocated list.
     */
    result->length = size;
    result->file = file;
    result->line = line;
    //memset ((char *) result->low_guard, GUARD_VALUE, GUARD_SIZE);
    //memset (result->body + size, GUARD_VALUE, GUARD_SIZE);
    result->flink = allocHead;
    result->blink = NULL;
    if (allocHead != NULL)
        allocHead->blink = result;
    allocHead = result;

    total_mallocs++;
    if (trace_on_at_malloc && (total_mallocs >= trace_on_at_malloc)) {
        //(void) fflush(stdout);
        //fprintf(stderr, "reached malloc trace enable point (%d)\n",
          //      total_mallocs);
        //fflush(stderr);
        alloc_tracing = TRUE;
        trace_on_at_malloc = 0;
    }

    /*
    if (alloc_tracing)
        fprintf(stderr,"ckalloc %lx %d %s %d\n", result->body, size, 
                file, line);
    */

    if (break_on_malloc && (total_mallocs >= break_on_malloc)) {
        break_on_malloc = 0;
        //(void) fflush(stdout);
        //fprintf(stderr,"reached malloc break limit (%d)\n", 
          //      total_mallocs);
        //fprintf(stderr, "program will now enter C debugger\n");
        //(void) fflush(stderr);
        //kill (getpid(), SIGINT);
    }

    current_malloc_packets++;
    if (current_malloc_packets > maximum_malloc_packets)
        maximum_malloc_packets = current_malloc_packets;
    current_bytes_malloced += size;
    if (current_bytes_malloced > maximum_bytes_malloced)
        maximum_bytes_malloced = current_bytes_malloced;

#if 0
    if (init_malloced_bodies)
        memset (result->body, 0xff, (int) size);
#endif
    return result->body;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbCkfree - debugging ckfree
 *
 *        Verify that the low and high guards are intact, and if so
 *        then free the buffer else panic.
 *
 *        The guards are erased after being checked to catch duplicate
 *        frees.
 *
 *        The second and third arguments are file and line, these contain
 *        the filename and line number corresponding to the caller.
 *        These are sent by the ckfree macro; it uses the preprocessor
 *        autodefines __FILE__ and __LINE__.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_DbCkfree(ptr, file, line)
    char *  ptr;
    char     *file;
    int       line;
{
    struct mem_header *memp = 0;  /* Must be zero for size calc */

    /*
     * Since header ptr is zero, body offset will be size
     */
    memp = (struct mem_header *)(((char *) ptr) - (int)memp->body);

    /*
    if (alloc_tracing)
        fprintf(stderr, "ckfree %lx %ld %s %d\n", memp->body, 
                memp->length, file, line);
    */

    if (validate_memory)
        Tcl_ValidateAllMemory (file, line);

    ValidateMemory (memp, file, line, TRUE);

    total_frees++;
    current_malloc_packets--;
    current_bytes_malloced -= memp->length;

    /*
     * Delink from allocated list
     */
    if (memp->flink != NULL)
        memp->flink->blink = memp->blink;
    if (memp->blink != NULL)
        memp->blink->flink = memp->flink;
    if (allocHead == memp)
        allocHead = memp->flink;
    My_Free((char *) memp);
    return 0;
}

/*
 *--------------------------------------------------------------------
 *
 * Tcl_DbCkrealloc - debugging ckrealloc
 *
 *	Reallocate a chunk of memory by allocating a new one of the
 *	right size, copying the old data to the new location, and then
 *	freeing the old memory space, using all the memory checking
 *	features of this package.
 *
 *--------------------------------------------------------------------
 */
#if 0
char *
Tcl_DbCkrealloc(ptr, size, file, line)
    char *ptr;
    unsigned int size;
    char *file;
    int line;
{
    char *new;

    new = Tcl_DbCkalloc(size, file, line);
    //memcpy((VOID *) new, (VOID *) ptr, (int) size);
    Tcl_DbCkfree(ptr, file, line);
    return(new);
}
#endif

/*
 *----------------------------------------------------------------------
 *
 * MemoryCmd --
 *     Implements the TCL memory command:
 *       memory info
 *       memory display
 *       break_on_malloc count
 *       trace_on_at_malloc count
 *       trace on|off
 *       validate on|off
 *
 * Results:
 *     Standard TCL results.
 *
 *----------------------------------------------------------------------
 */
	/* ARGSUSED */
static int
MemoryCmd (clientData, interp, argc, argv)
    char       *clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
#if 0
    char *fileName;

    if (argc < 2) {
	//Tcl_AppendResult(interp, "wrong # args:  should be \"",
		//argv[0], " option [args..]\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (my_strcmp(argv[1],"trace") == 0) {
        if (argc != 3) 
            goto bad_suboption;
        alloc_tracing = (my_strcmp(argv[2],"on") == 0);
        return TCL_OK;
    }
    if (my_strcmp(argv[1],"init") == 0) {
        if (argc != 3)
            goto bad_suboption;
        init_malloced_bodies = (my_strcmp(argv[2],"on") == 0);
        return TCL_OK;
    }
    if (my_strcmp(argv[1],"validate") == 0) {
        if (argc != 3)
             goto bad_suboption;
        validate_memory = (my_strcmp(argv[2],"on") == 0);
        return TCL_OK;
    }
    if (my_strcmp(argv[1],"trace_on_at_malloc") == 0) {
        if (argc != 3) 
            goto argError;
        if (Tcl_GetInt(interp, argv[2], &trace_on_at_malloc) != TCL_OK)
                return TCL_ERROR;
         return TCL_OK;
    }
    if (my_strcmp(argv[1],"break_on_malloc") == 0) {
        if (argc != 3) 
            goto argError;
        if (Tcl_GetInt(interp, argv[2], &break_on_malloc) != TCL_OK)
                return TCL_ERROR;
        return TCL_OK;
    }

    if (my_strcmp(argv[1],"info") == 0) {
        dump_memory_info(stdout);
        return TCL_OK;
    }
    if (my_strcmp(argv[1],"active") == 0) {
        if (argc != 3) {
	    //Tcl_AppendResult(interp, "wrong # args:  should be \"",
		    //argv[0], " active file", (char *) NULL);
	    return TCL_ERROR;
	}
        fileName = argv [2];
        if (fileName [0] == '~')
            if ((fileName = Tcl_TildeSubst (interp, fileName)) == NULL)
                return TCL_ERROR;
        if (Tcl_DumpActiveMemory (fileName) != TCL_OK) {
	    //Tcl_AppendResult(interp, "error accessing ", argv[2], 
		    //(char *) NULL);
	    return TCL_ERROR;
	}
	return TCL_OK;
    }
    //Tcl_AppendResult(interp, "bad option \"", argv[1],
	    //"\":  should be info, init, active, break_on_malloc, ",
	    //"trace_on_at_malloc, trace, or validate", (char *) NULL);
    return TCL_ERROR;

argError:
    //Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[0],
	    //" ", argv[1], "count\"", (char *) NULL);
    return TCL_ERROR;

bad_suboption:
    //Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[0],
	    //" ", argv[1], " on|off\"", (char *) NULL);
    return TCL_ERROR;
#endif
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InitMemory --
 *     Initialize the memory command.
 *
 *----------------------------------------------------------------------
 */
void
Tcl_InitMemory(interp)
    Tcl_Interp *interp;
{
#if 0
Tcl_CreateCommand (interp, "memory", MemoryCmd, (ClientData)NULL, 
                  (void (*)())NULL);
#endif
}

#else


/*
 *----------------------------------------------------------------------
 *
 * Tcl_Ckalloc --
 *     Interface to malloc when TCL_MEM_DEBUG is disabled.  It does check
 *     that memory was actually allocated.
 *
 *----------------------------------------------------------------------
 */
VOID *
Tcl_Ckalloc (size)
    unsigned int size;
{
        char *result;

        result = Tcl_Malloc(size);
        //if (result == NULL) 
          //      panic("unable to alloc %d bytes", size);
        return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TckCkfree --
 *     Interface to free when TCL_MEM_DEBUG is disabled.  Done here rather
 *     in the macro to keep some modules from being compiled with 
 *     TCL_MEM_DEBUG enabled and some with it disabled.
 *
 *----------------------------------------------------------------------
 */
void
Tcl_Ckfree (ptr)
    VOID *ptr;
{
        Tcl_Free (ptr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InitMemory --
 *     Dummy initialization for memory command, which is only available 
 *     if TCL_MEM_DEBUG is on.
 *
 *----------------------------------------------------------------------
 */
	/* ARGSUSED */
void
Tcl_InitMemory(interp)
    Tcl_Interp *interp;
{
}

#endif
