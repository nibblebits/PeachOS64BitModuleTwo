#include "streamer.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "config.h"
#include "kernel.h"
#include "status.h"

#include <stdbool.h>

struct disk_stream_cache* diskstreamer_cache_new()
{
    struct disk_stream_cache* cache = kzalloc(sizeof(struct disk_stream_cache));
    if (!cache)
    {
        goto out;
    }

out:
    return cache;
}

struct disk_stream_cache_bucket_level1* diskstreamer_cache_bucket_level1_get(struct disk_stream_cache* cache, int index)
{
    struct disk_stream_cache_bucket_level1* level1 = NULL;
    if (cache->total <= index)
    {
        size_t old_total = cache->total;
        size_t new_total = index+1;
        size_t new_size = new_total * sizeof(struct disk_Stream_cache_bucket_level1**);

        struct disk_stream_cache_bucket_level1 **new_buckets = krealloc(cache->buckets, new_size);
        if (!new_buckets)
        {
            return NULL;
        }

        memset(new_buckets+old_total, 0, (new_total-old_total) * sizeof(*new_buckets));
        cache->buckets = new_buckets;
        cache->total = new_total;
    }

    level1 = cache->buckets[index];
    if (!level1)
    {
        cache->buckets[index] = kzalloc(sizeof(struct disk_stream_cache_bucket_level1));
        if (!cache->buckets[index])
        {
            return NULL;
        }

        level1 = cache->buckets[index];
        level1->total_buckets = 0;
    }

    return level1;
}

struct disk_stream_cache_bucket_level2* diskstreamer_cache_bucket_level2_get(struct disk_stream_cache_bucket_level1* level1_bucket, int index)
{
    int max_buckets = (sizeof(level1_bucket->buckets) / sizeof(*level1_bucket->buckets));
    if (index >= max_buckets)
    {
        return NULL;
    }

    struct disk_stream_cache_bucket_level2* level_two = level1_bucket->buckets[index];
    if (!level_two)
    {
        level_two = kzalloc(sizeof(struct disk_stream_cache_bucket_level2));
        if (!level_two)
        {
            return NULL;
        }

        level_two->total_buckets = 0;
        level1_bucket->buckets[index] = level_two;
    }

    return level_two;
}

struct disk_stream_cache_bucket_level3* diskstreamer_cache_bucket_level3_get(struct disk_stream_cache_bucket_level2* level2, int index)
{
    struct disk_stream_cache_bucket_level3* level3 = NULL;
    int max_buckets = (sizeof(level2->buckets) / sizeof(*level2->buckets));
    if (index >= max_buckets)
    {
        return NULL;
    }

    level3 = level2->buckets[index];
    if (!level3)
    {
        level3 = kzalloc(sizeof(struct disk_stream_cache_bucket_level3));
        if (!level3)
        {
            return NULL;
        }
        level3->total_sectors = 0;
        level2->buckets[index] = level3;
    }
    return level3;
}

void diskstreamer_cache_bucket_level3_free(struct disk_stream_cache_bucket_level3* level3)
{
    for(size_t i = 0; i < DISK_STREAM_LEVEL3_SECTORS_ARRAY_SIZE; i++)
    {
        kfree(level3->sectors[i]);
    }
    kfree(level3);
}

struct disk_stream* diskstreamer_new(int disk_id)
{
    struct disk* disk = disk_get(disk_id);
    if (!disk)
    {
        return 0;
    }

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

struct disk_stream* diskstreamer_new_from_disk(struct disk* disk)
{
    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total)
{
    int sector = stream->pos / PEACHOS_SECTOR_SIZE;
    int offset = stream->pos % PEACHOS_SECTOR_SIZE;
    int total_to_read = total;
    bool overflow = (offset+total_to_read) >= PEACHOS_SECTOR_SIZE;
    char buf[PEACHOS_SECTOR_SIZE];
    if (overflow)
    {
        total_to_read -= (offset+total_to_read) - PEACHOS_SECTOR_SIZE;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0)
    {
        goto out;
    }

    for(int i = 0; i < total_to_read; i++)
    {
        *(char*)out++ = buf[offset+i];
    }

    stream->pos += total_to_read;
    if (overflow)
    {
        res = diskstreamer_read(stream, out, total-total_to_read);
    }
    
out:
    return res;
}

void diskstreamer_close(struct disk_stream* stream)
{
    kfree(stream);
}