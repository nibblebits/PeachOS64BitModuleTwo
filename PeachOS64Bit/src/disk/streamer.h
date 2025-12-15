#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "disk.h"

#define DISK_STREAMER_CACHE_STATUS_NEW_CACHE_ENTRY 0x01
#define DISK_STREAMER_CACHE_STATUS_CACHE_FOUND 0x00

#define DISK_STREAMER_MAX_CACHE_SECTOR_SIZE 2048

// 64 cache sectors per bucket
#define DISK_STREAM_LEVEL3_SECTORS_ARRAY_SIZE 64
#define DISK_STREAM_BUCKET_ARRAY_SIZE 1024
#define DISK_STREAM_CACHE_ROUNDROBIN_MAX 1024

#define DISK_STREAM_BUCKET3_BYTE_SIZE(sector_size) (1L * DISK_STREAM_LEVEL3_SECTORS_ARRAY_SIZE * (sector_size))
#define DISK_STREAM_BUCKET2_BYTE_SIZE(sector_size) (1L * DISK_STREAM_BUCKET3_BYTE_SIZE(sector_size) * DISK_STREAM_BUCKET_ARRAY_SIZE)
#define DISK_STREAM_BUCKET1_BYTE_SIZE(sector_size) (1L * DISK_STREAM_BUCKET2_BYTE_SIZE(sector_size) * DISK_STREAM_BUCKET_ARRAY_SIZE)

struct disk_stream_cache_sector
{
    char buf[DISK_STREAMER_MAX_CACHE_SECTOR_SIZE];
};

struct disk_stream_cache_bucket_level3
{
    struct disk_stream_cache_sector* sectors[DISK_STREAM_LEVEL3_SECTORS_ARRAY_SIZE];
    size_t total_sectors;

    int roundrobin_count;
};

struct disk_stream_cache_bucket_level2
{
    struct disk_stream_cache_bucket_level3* buckets[DISK_STREAM_BUCKET_ARRAY_SIZE];
    size_t total_buckets;
};

struct disk_stream_cache_bucket_level1
{
    struct disk_stream_cache_bucket_level2* buckets[DISK_STREAM_BUCKET_ARRAY_SIZE];
    size_t total_buckets;
};

struct disk_stream_cache_round_robin
{
    struct disk_stream_cache_bucket_level3* queue[DISK_STREAM_CACHE_ROUNDROBIN_MAX];
    int pos;
};

struct disk_stream_cache
{
    // level1 buckets point to 32 GB 
    struct disk_stream_cache_bucket_level1** buckets;
    size_t total;

    struct disk_stream_cache_round_robin mem_roundrobin;
};

struct disk_stream
{
    int pos;
    struct disk* disk;
};

// postion = 5484847
// level 1 index = 0 // 32 GB 
// level 2 index = 30 (32768 * 1024 = 33554432 (32 MB))
// level3  indx = 2 (512 * 64 = 32768)

struct disk_stream* diskstreamer_new(int disk_id);
int diskstreamer_seek(struct disk_stream* stream, int pos);
int diskstreamer_read(struct disk_stream* stream, void* out, int total);
void diskstreamer_close(struct disk_stream* stream);
struct disk_stream* diskstreamer_new_from_disk(struct disk* disk);

#endif