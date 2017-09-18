#include <stdio.h>
#include <stdlib.h>
typedef unsigned char uint8;



struct decive_char_table{
	uint8 	pid[6];
	uint8	bcr;
	uint8	dcr;
	uint8	dynamic_address;

};
struct device_characteristic_table_t
{
	uint8	*device_status;
	int		size;
};

struct i3c_device_characteristic_t
{
	uint8 	pid[6];
	uint8	bcr;
	uint8	dcr;
	uint8	dynamic_address;
	i3c_device_characteristic_t *next_device;
};

struct device_characteristic_table_t * newdevice_characteristic_table (int sz) {
    // Try to allocate device_characteristic_table_t structure.

    struct device_characteristic_table_t *retVal = malloc (sizeof (struct device_characteristic_table_t));
    if (retVal == NULL)
        return NULL;

    // Try to allocate device_characteristic_table_t device_status, free structure if fail.

    retVal->device_status = malloc (sz * sizeof (uint8) * 9);
    if (retVal->device_status == NULL) {
        free (retVal);
        return NULL;
    }

    // Set size and return.

    retVal->size = sz;
    return retVal;
}

void create_root_device (i3c_device_characteristic_t * device) {
	int index =0;
    /* Creates a node at the end of the list */
    device = malloc (sizeof (struct i3c_device_characteristic_t));
	device->next_device = 0;

    if ( device == NULL )
    {
        printf( "Out of memory" );
        return;
    }
    /* initialize the new memory */
    device->next_device = 0;
	for(index = 0; index < 6; index++){
		device->pid[index] = 0x00;
	}
	device->bcr = 0x00;
	device->dcr = 0x00;
	device->dynamic_address = 0xFF;
    return;
}

void newdevice_at_end (i3c_device_characteristic_t * device) {
	int index =0;
    struct i3c_device_characteristic_t *retVal;
	retVal = device;
	if ( retVal != 0 ) {
        while ( retVal->next_device != 0)
        {
            retVal = retVal->next_device;
        }
    }
    /* Creates a node at the end of the list */
    retVal->next_device = malloc (sizeof (struct i3c_device_characteristic_t));
	retVal = retVal->next_device;

    if ( retVal == 0 )
    {
        printf( "Out of memory" );
        return;
    }
    /* initialize the new memory */
    retVal->next_device = 0;
	for(index = 0; index < 6; index++){
		retVal->pid[index] = 0x00;
	}
	retVal->bcr = 0x00;
	retVal->dcr = 0x00;
	retVal->dynamic_address = 0xFF;
    return;
}

void deldevice_characteristic_table_t (struct device_characteristic_table_t *device_characteristic_table) {
    // Can safely assume device_characteristic_table is NULL or fully built.

    if (device_characteristic_table != NULL) {
        free (device_characteristic_table->device_status);
        free (device_characteristic_table);
    }
}