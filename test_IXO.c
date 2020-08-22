#include "IXO.h"

#include <stdint.h>
#include <stdlib.h>

/*

Sample JSON Format
Person
{
    "name": String,
    "age": Int
}

Sample JSON

{
    "name": "Alice",
    "age": 10
}

-*/

typedef struct Person
{
    char* name;
    uint64_t age;
    float velocity[3];
} Person;

IXO_TupleField vec3f_fields[] = {
    {0*sizeof(float), &IXO_number_class[IXO_NUM_FLOAT]},
    {1*sizeof(float), &IXO_number_class[IXO_NUM_FLOAT]},
    {2*sizeof(float), &IXO_number_class[IXO_NUM_FLOAT]},
    {0}
};
IXO_Class vec3f_class = {
    .type_tuple = {
        .type   = IXO_CLASS_TUPLE,
        .fields = vec3f_fields
    }
};



IXO_StructField person_fields[] = {
    {"name",     offsetof(Person,name),     &IXO_string_class},
    {"age",      offsetof(Person,age),      &IXO_number_class[IXO_NUM_UINT64]},
    {"velocity", offsetof(Person,velocity), &vec3f_class },
    {0}
};

IXO_Class person_class = {
    .type_struct = {
        .type   = IXO_CLASS_STRUCT,
        .fields = person_fields,
    }
};


int main()
{
    FILE* file = fopen("./tst/tst1.json", "r");
    if(!file) return 1;
    IXO_DesCtx des;
    IXO_DesConstruct(&des, file, IXO_JSON);

    Person the_person = {0};
    if(IXO_DesReadObj(&des, &the_person, &person_class)==0)
    {
        printf("JSON Object couldn't be read\n");
    };
    IXO_DesDestruct(&des);

    printf("Name: %s, Age: %i, Velocity: [%f, %f, %f]",
           the_person.name,
           (int)the_person.age,
           the_person.velocity[0],
           the_person.velocity[1],
           the_person.velocity[2]);

    free(the_person.name);
}
