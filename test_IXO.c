#include "IXO.h"

#include <stdint.h>
#include <stdlib.h>

/*

Sample JSON Format
Person
{
    "name": String,
    "age": Int,
    "velocity": [ Float, Float, Float ]
}

Sample JSON

{
    "name": "Alice",
    "age": 10,
    "velocity": [ 10, 1.5, -42 ]
}


-*/


typedef struct TST_Vec3f
{
    float x, y, z;
} TST_Vec3f;

IXO_TUPLEDEF(TST_Vec3f,
    (x, &IXO_float_class),
    (y, &IXO_float_class),
    (z, &IXO_float_class)
);


typedef struct TST_Person
{
    char* name;
    uint64_t age;
    TST_Vec3f velocity;
} TST_Person;

IXO_STRUCTDEF(TST_Person,
    (name,     &IXO_string_class),
    (age,      &IXO_uint64_class),
    (velocity, &TST_Vec3f_class)
);



int main()
{
    FILE* file = fopen("./tst/tst1.json", "r");
    if(!file) return 1;
    IXO_DesCtx des;
    IXO_DesConstruct(&des, file, IXO_JSON);

    TST_Person the_person = {0};
    if(IXO_DesReadObj(&des, &the_person, &TST_Person_class)==0)
    {
        printf("JSON Object couldn't be read\n");
    };
    IXO_DesDestruct(&des);

    printf("Name: %s, Age: %i, Velocity: [%f, %f, %f]",
           the_person.name,
           (int)the_person.age,
           the_person.velocity.x,
           the_person.velocity.y,
           the_person.velocity.z);

    free(the_person.name);
}
