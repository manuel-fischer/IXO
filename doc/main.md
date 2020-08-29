# The IXO Input-Output Library Documentation

## Hello World example - Reading

**test.json**
```json
{
    "name": "Alice",
    "age": 42
}
```



**main.c**
```c
#include <IXO.h>

#include <stdint.h> // -> uint64_t
#include <stdio.h>  // -> FILE

typedef struct Person
{
    char* name; // Needs to be freed
    uint64_t age;
} Person;

IXO_STRUCTDEF(Person,
    (name, &IXO_string_class),
    (age,  &IXO_uint64_class)
);

int main()
{
    TST_Person the_person = {0};
    if(!IXO_Read("test.json", &the_person, &Person_class))
    {
        printf("Object couldn't be read completely\n");
    }
    else
    {
        printf("%s is %" PRIu64 " years old\n", the_person.name, the_person.age);
    }
    
    free(the_person.name);
}
```

### Step by step explanation
#### 1. Inclusion of `IXO.h`
```c
#include <IXO.h>
```
It is sufficient to include `IXO.h` to use the library, specialized files like
`IXO_classdef.h` could be included in files that only define type information
but do not read/write any objects.

#### 2. Defining the type information
```c
IXO_STRUCTDEF(Person,
    (name, &IXO_string_class),
    (age,  &IXO_uint64_class)
);
```

To deserialize or serialize objects, type information needs to be defined.
This is done by various macros like `IXO_STRUCTDEF`, `IXO_TUPLEDEF`,
`IXO_BITDEF`, `IXO_ENUMDEF`.

Each macro defines type information in a variable of type `IXO_Class`. The name
itself is the name of the given type appended with `_name`. This means in the
example above, the variable `Person_class` is defined.

The Macros takes the name of the type as the first parameter, that should be
one token, after that a variable number of tuples follows. Each tuple has
a specific number of arguments. Here the name of a struct type is given as
the first parameter to `IXO_STRUCTDEF`. `IXO_STRUCTDEF` accepts tuples with
two arguments, the first element is the name of the member in the struct, and
the second element is a pointer the IXO-class, which is used do deserialize
the given field itself.

There are some builtin classes for primitive numeric types:

```
IXO_uint8_class
IXO_uint16_class
IXO_uint32_class
IXO_uint64_class
IXO_int8_class
IXO_int16_class
IXO_int32_class
IXO_int64_class
IXO_float_class
IXO_double_class
```

And also there is a class for zero terminated strings, corresponding to the
`char*` type:

```
IXO_string_class
```

It dynamically allocates the string with `malloc` (or related), so the
string needs to be cleaned with `free`.

**For headerfiles**

To declare the class without implementing it, the macro `IXO_DECLARE_CLASS`
can be used with the type name. Here it would be `IXO_DECLARE_CLASS(Person);`.

Note the trailing semicolon, semicolons are needed after any of these macros.

#### 3. Reading the object
```c
TST_Person the_person = {0};
if(!IXO_Read("test.json", &the_person, &Person_class))
{
    printf("Object couldn't be read completely\n");
}
else
{
    printf("%s is %" PRIu64 " years old\n", the_person.name, the_person.age);
}
```

To read an object from a file, the function `IXO_Read` is called.
The first parameter is the filename. The second
is a pointer to the object that gets filled with data. The last parameter
is the class that provides type information of the object.
The function auto-detects the file type from the filename, that is
corresponding to the enum value `IXO_JSON`.

The object itself should be filled with the default values.

The function `IXO_Read_FILE` takes a `FILE*` instead of a filename. The file
type cannot be detected in this case, so an additional `IXO_FileType` parameter
should be passed.

### Structs vs Tuples

There are two ways to define the class for a struct type. These correspond to
the macros `IXO_STRUCTDEF` and `IXO_TUPLEDEF`. The difference is the
JSON-representation. `IXO_STRUCTDEF` defines the type information that is
needed to load an object from a JSON-object (denoted by `{` and `}`).
A string for each member is stored.
`IXO_TUPLEDEF` defines the type information that is needed to load an object
from a JSON-array (denoted by `[` and `]`). No strings for the members are
created. The elements are loaded sequencially in the order in which the
members are defined in the macro. Using a tuple instead of a struct might be
useful for coordinates/vectors or repeated elements like table rows.
Reading/writing objects as tuples from/to json might be more efficient than
reading/writing as structs(JSON-objects), because the name of each field
is not stored and is not looked up to find the type information of that
specific member. On the other side using structs is more portable when the
order of elements changes or elements are removed or added.