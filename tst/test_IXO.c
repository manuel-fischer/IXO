#include "IXO.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <memory.h>

/*

Sample JSON Format
Person
{
    "name": String,
    "age": Int,
    "car": {
        "flags": ["AUTOMATIC_DOORS", "CAN_FLY", "HAS_RADIO", "ELECTRO"],
        "velocity": [ Float, Float, Float ]
    }
}

Sample JSON

{
    "name": "Alice",
    "age": 10,
    "car": {
        "flags": ["CAN_FLY", "ELECTRO"],
        "velocity": [ 10, 1.5, -42 ]
    }
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

typedef uint32_t TST_CarType;
enum
{
    TST_OTHER_CAR,
    TST_VAN,
    TST_TRUCK,
    TST_ROADSTER,
    TST_SPORTSCAR
};

IXO_ENUMDEF(TST_CarType,
    (OTHER_CAR, TST_OTHER_CAR),
    (VAN,       TST_VAN),
    (TRUCK,     TST_TRUCK),
    (ROADSTER,  TST_ROADSTER),
    (SPORTSCAR, TST_SPORTSCAR)
);

#define TST_CAR_TYPE_STR(type) \
   ("Other\0..." \
    "Van\0....." \
    "Truck\0..." \
    "Roadster\0" \
    "Sportscar"+((type)*9))

typedef uint32_t TST_CarFlags;
#define TST_AUTOMATIC_DOORS  1u
#define TST_CAN_FLY          2u
#define TST_HAS_RADIO        4u
#define TST_ELECTRO          8u

IXO_BITDEF(TST_CarFlags,
    (AUTOMATIC_DOORS,  TST_AUTOMATIC_DOORS),
    (CAN_FLY,          TST_CAN_FLY),
    (HAS_RADIO,        TST_HAS_RADIO),
    (ELECTRO,          TST_ELECTRO)
);

typedef struct TST_Car
{
    TST_CarType  type;
    TST_CarFlags flags;
    TST_Vec3f    velocity;
} TST_Car;

IXO_STRUCTDEF(TST_Car,
    (type,     &TST_CarType_class),
    (flags,    &TST_CarFlags_class),
    (velocity, &TST_Vec3f_class)
);

typedef struct TST_StringArray
{
    size_t size;
    size_t capacity;
    char** data;
} TST_StringArray;

void* TST_StringArray_vpush(void* array, void* data)
{
    TST_StringArray* arr = (TST_StringArray*)array;
    if(arr->size == arr->capacity)
    {
        size_t new_cap = arr->capacity;
        if(new_cap == 0) new_cap = 16;
        else new_cap <<= 1;
        char** new_data = realloc(arr->data, sizeof(char*)*new_cap);
        if(new_data == 0)
        {
            if(data != NULL)
                free(*(char**)data);
            return NULL;
        }
        arr->data = new_data;
        arr->capacity = new_cap;
    }
    ++arr->size;
    char** elem = &arr->data[arr->size-1];
    if(data != NULL)
        memcpy(elem, data, sizeof(*elem));
    else
        memset(elem, 0, sizeof(*elem));
    return elem;
}

void* TST_StringArray_vnext(void* array, void* prev)
{
    TST_StringArray* arr = (TST_StringArray*)array;

    if(arr->size == 0)
        return NULL;
    if(prev == NULL)
        return arr->data;
    if((char**)prev + 1 == arr->data+arr->size)
        return NULL;
    return (void*)((char**)prev + 1);
}

IXO_ARRAYDEF(TST_StringArray,
    &IXO_string_class,
    sizeof(char*),
    &TST_StringArray_vpush,
    &TST_StringArray_vnext
);

typedef struct TST_Person
{
    char* name;
    uint64_t age;
    TST_Car car;
    TST_StringArray hobbies;
} TST_Person;

IXO_STRUCTDEF(TST_Person,
    (name,     &IXO_string_class),
    (age,      &IXO_uint64_class),
    (car,      &TST_Car_class),
    (hobbies,  &TST_StringArray_class)
);



int main()
{
    static const char filename[] = "./tst/tst1.json";

    TST_Person the_person = {0};
    if(IXO_Read(filename, &the_person, &TST_Person_class)==0)
    {
        printf("JSON Object couldn't be read completely\n");
    }

    printf("Name: %s\nAge: %" PRIu64 "\n",
           the_person.name,
           the_person.age);

    printf("Car:\n  Type: %s\n  Velocity: [%f, %f, %f]\n  Flags: %i\n",
           TST_CAR_TYPE_STR(the_person.car.type),
           the_person.car.velocity.x,
           the_person.car.velocity.y,
           the_person.car.velocity.z,
           the_person.car.flags);

    printf("Hobbies:\n");
    for(size_t i = 0; i < the_person.hobbies.size; ++i)
    {
        printf("  %i. %s\n", i+1, the_person.hobbies.data[i]);
    }

    for(size_t i = 0; i < the_person.hobbies.size; ++i)
    {
        free(the_person.hobbies.data[i]);
    }
    free(the_person.hobbies.data);
    free(the_person.name);
}
