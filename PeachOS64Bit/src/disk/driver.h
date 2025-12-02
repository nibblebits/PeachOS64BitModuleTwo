#ifndef KERNEL_DISK_DRIVER_H
#define KERNEL_DISK_DRIVER_H
#include <stdbool.h>
#include <stdint.h>

#define DISK_DRIVER_NAME_SIZE 20
struct disk;
struct disk_driver;

typedef int (*DISK_DRIVER_LOADED)(struct disk_driver* driver);
typedef void (*DISK_DRIVER_UNLOADED)(struct disk_driver* driver);

typedef int (*DISK_DRIVER_MOUNT)(struct disk_driver* driver);
typedef void (*DISK_DRIVER_UNMOUNT)(struct disk* disk);

typedef int (*DISK_DRIVER_WRITE)(struct disk* disk, unsigned int lba, int total_sectors, void* buf_in);
typedef int (*DISK_DRIVER_READ)(struct disk* disk, unsigned int lba, int total_sectors, void* buf_out);
typedef int (*DISK_DRIVER_MOUNT_PARTITION)(struct disk* disk, long starting_lba, long ending_lba, struct disk** partition_disk_out);

struct disk_driver
{
    char name[DISK_DRIVER_NAME_SIZE];

    struct 
    {
        DISK_DRIVER_LOADED loaded;
        DISK_DRIVER_UNLOADED unloaded;
        DISK_DRIVER_MOUNT mount;
        DISK_DRIVER_UNMOUNT unmount;
        DISK_DRIVER_WRITE write;
        DISK_DRIVER_READ read;

        // Inheritor shall create a virtual disk when this is called
        // virtual disk shall point to the lba start and lba end.
        DISK_DRIVER_MOUNT_PARTITION mount_partition;
        
    } functions;

    // Private data the disk driver may use for its self
    void* private;
};
#endif