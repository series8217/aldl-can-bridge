#include "linuxaldl.h"
#include "definitions/all.h"


aldl_definition_t* aldl_definition_table[2] = { &aldl_DF, NULL };
linuxaldl_settings aldl_settings = { .aldlportname=NULL,
                                     .faldl=0,
                                     .logfilename=NULL,
                                     .flogfile=1,
                                     .scanning=0,
                                     .aldldefname=NULL,
                                     .definition=NULL,
                                     .aldl_definition_table=aldl_definition_table,
                                     .data_set_raw=NULL,
                                     .data_set_strings=NULL,
                                     .data_set_floats=NULL,
                                     .scan_interval=150,
                                     .scan_timeout=100
                                     };
