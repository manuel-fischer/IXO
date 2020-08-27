#include "IXO.h"

#include <stdint.h>
#include <stdlib.h>

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

typedef struct TST_Person
{
    char* name;
    uint64_t age;
    TST_Car car;
} TST_Person;

IXO_STRUCTDEF(TST_Person,
    (name,     &IXO_string_class),
    (age,      &IXO_uint64_class),
    (car,      &TST_Car_class)
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
        printf("JSON Object couldn't be read completely\n");
    };
    IXO_DesDestruct(&des);

    printf("Name: %s, Age: %i\n",
           the_person.name,
           (int)the_person.age);

    printf("Type: %s, Velocity: [%f, %f, %f], Flags: %i\n",
           TST_CAR_TYPE_STR(the_person.car.type),
           the_person.car.velocity.x,
           the_person.car.velocity.y,
           the_person.car.velocity.z,
           the_person.car.flags);

    free(the_person.name);
}
