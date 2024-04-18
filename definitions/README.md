## WRITING A DEFINITION FOR LINUXALDL
Labels/names/units must not contain white space, commas, newlines, 
or double quotes.
They may contain: a-z A-Z 0-9 ~!@#$%^&*()-=_+|\/}{:<>?.[];'
Label/names/units may not be NULL, except for units for seperators,
and in the last element of the mode1_def[] array. (see below)

The last element of the mode1_def[] array must be LINUXALDL_MODE1_END_DEF
(which is a byte_def_t with label and units NULL and all other values 0).

After adding a definition file, include it in all.h and add a pointer to
the definition object in aldl_definition_table.

See the df.h definition file for a complete example of a definition.