#include "disk.h"
#include "io/io.h"
#include "config.h"
#include "status.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "string/string.h"
#include "lib/vector/vector.h"
#include "disk/streamer.h"

struct vector* disk_vector = NULL;

// A pointer to the primary hard disk
// allowing IO directly to the disk from LBA zero onwards
struct disk* disk = NULL;

// A pointer to the virtual disk that contains the primary kernel filesystem
// where kernel files are found.
struct disk* primary_fs_disk = NULL;

// int disk_read_sector(int lba, int total, void* buf)
// {
//     // Wait for the disk not to be busy
//     while(insb(0x1F7) & 0x80)
//     {
//         // spin
//     }

//     // Select drive: bits 7..4=0xE, bits 3..0 = high nibble of lba
//     outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
//     // Sector count
//     outb(0x1F2, (unsigned char) total);
//     // LBA low, mid, high
//     outb(0x1F3, (unsigned char)(lba & 0xff));
//     outb(0x1F4, (unsigned char)((lba >> 8) & 0xff));
//     outb(0x1F5, (unsigned char)((lba >> 16) & 0xff));

//     // Read SECTORS command (0x20)
//     outb(0x1F7, 0x20);

//     unsigned short* ptr = (unsigned short*) buf;
//     for (int b = 0; b < total; b++)
//     {
//         // Wait for the disk not to be busy
//         while(insb(0x1F7) & 0x80)
//         {
//             // spin
//         }

//         // Check error bit
//         char status = insb(0x1F7);
//         if (status & 0x01)
//         {
//             return -EIO;
//         }

//         // Wait for the buffer to be ready
//         while(!(insb(0x1F7) & 0x08))
//         {
//             // spin
//         }

//         // Copy from hard disk to memory
//         for (int word = 0; word < 256; word++)
//         {
//             *ptr++ = insw(0x1F0);
//         }

//     }
//     return 0;
// }

struct disk* disk_hardware_disk(struct disk* disk)
{
    return disk->hardware_disk;
}

int disk_create_partition(struct disk* disk, int starting_lba, int ending_lba, struct disk** partition_disk_out)
{
    return disk_driver_mount_partition(disk->driver, disk, starting_lba, ending_lba, partition_disk_out);
}

int disk_filesystem_mount(struct disk* disk)
{
    disk->filesystem = fs_resolve(disk);
    if (disk->filesystem)
    {
        char fs_name[11] = {0};
        char primary_drive_fs_name[11] = {0};
        strncpy(primary_drive_fs_name, PEACHOS_KERNEL_FILESYSTEM_NAME, strlen(PEACHOS_KERNEL_FILESYSTEM_NAME));
        disk->filesystem->volume_name(disk->fs_private, fs_name, sizeof(fs_name));
        if (strncmp(fs_name, primary_drive_fs_name, sizeof(fs_name)) == 0)
        {
            primary_fs_disk = disk;
        }
    }

    return 0;
}

long disk_real_sector(struct disk* idisk, unsigned int lba)
{
    size_t absolute_lba = idisk->starting_lba+lba;
    return absolute_lba;
}

long disk_real_offset(struct disk* idisk, unsigned int lba)
{
    size_t absolute_lba = disk_real_sector(idisk, lba);
    return absolute_lba * idisk->sector_size;
}

int disk_create_new(struct disk_driver* driver, struct disk* hardware_disk, int type, int starting_lba, int ending_lba, size_t sector_size, void* driver_private_data, struct disk** disk_out)
{
    int res = 0;
    struct disk* disk = kzalloc(sizeof(struct disk));
    if (!disk)
    {
        res = -ENOMEM;
        goto out;
    }

    if (hardware_disk && type == PEACHOS_DISK_TYPE_REAL)
    {
        res = -EINVARG;
        goto out;
    }

    if (type == PEACHOS_DISK_TYPE_REAL)
    {
        hardware_disk = disk;
    }

    if (hardware_disk == NULL)
    {
        res = -EINVARG;
        goto out;
    }

    if (hardware_disk->type != PEACHOS_DISK_TYPE_REAL)
    {
        res = -EINVARG;
        goto out;
    }

    disk->type = type;
    disk->id = vector_count(disk_vector);
    disk->sector_size = sector_size;
    disk->starting_lba = starting_lba;
    disk->ending_lba = ending_lba;
    disk->driver = driver;
    disk->driver_private = driver_private_data;
    disk->hardware_disk = hardware_disk;
    disk->cache = diskstreamer_cache_new();

    if (disk_out)
    {
        *disk_out = disk;
    }
    vector_push(disk_vector, &disk);
out:
    return res;
}

int disk_mount_all()
{
    int res = 0;
    res = disk_driver_mount_all();
    return res;
}

int disk_search_and_init()
{
    int res = 0;

    res = disk_driver_system_init();
    if (res < 0)
    {
        res = -EIO;
        goto out;
    }

    disk_vector = vector_new(sizeof(struct disk*), 4, 0);
    if (!disk_vector)
    {
        res = -ENOMEM;
        goto out;
    }

    res = disk_mount_all();
    if (res < 0)
    {
        goto out;
    }
out:
    return res;
}

struct disk* disk_primary()
{
    return disk;
}

struct disk* disk_primary_fs_disk()
{
    return primary_fs_disk;
}



struct disk* disk_get(int index)
{
    size_t total_disks = vector_count(disk_vector);
    if (index >= (int) total_disks)
    {
        // out of bounds no such disk is loaded
        return NULL;
    }

    struct disk* disk = NULL;
    vector_at(disk_vector, index, &disk, sizeof(disk));
    return disk;
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf)
{
    size_t absolute_lba = idisk->starting_lba + lba;
    size_t absolute_ending_lba = absolute_lba + total;
    if (absolute_ending_lba > idisk->ending_lba)
    {
        // Is this the primary disk
        if (idisk->starting_lba != 0 && idisk->ending_lba != 0)
        {
            // Out of bounds, you cannot read over to other virtual disks
            return -EIO;
        }
    }

    if (!idisk->driver->functions.read)
    {
        return -EIO;
    }

    return idisk->driver->functions.read(idisk, absolute_lba, total, buf);
}

void* disk_private_data_driver(struct disk* disk)
{
    return disk->driver_private;
}
