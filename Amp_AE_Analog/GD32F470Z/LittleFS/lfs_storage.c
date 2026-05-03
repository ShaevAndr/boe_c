
#include "logger.h"
#include "lfs_storage.h"

static uint8_t read_buffer[LFS_CACHE_SIZE];
static uint8_t prog_buffer[LFS_CACHE_SIZE];
static uint8_t lookahead_buffer[LFS_LOOKAHEAD_SIZE];

lfs_t main_storage;

static int lfs_device_read(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, void *buffer, lfs_size_t size)
{
  int res = mtdRead((TMTDDevice*)c->context, (block * c->block_size) + off, buffer, size);
  if (res > 0) res = 0;
  return res;
}

static int lfs_device_prog(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, const void *buffer, lfs_size_t size)
{
  int res = mtdWrite((TMTDDevice*)c->context, (block * c->block_size) + off, buffer, size);
  if (res > 0) res = 0;
  return 0;
}

static int lfs_device_erase(const struct lfs_config *c, lfs_block_t block)
{
  //TMTDDevice *dev = (TMTDDevice*)c->context;
  return 0;
}

static int lfs_device_sync(const struct lfs_config *c)
{
  return 0;
}

// configuration of the filesystem is provided by this struct
static struct lfs_config lfs_cfg = {
  // block device operations
  .read  = lfs_device_read,
  .prog  = lfs_device_prog,
  .erase = lfs_device_erase,
  .sync  = lfs_device_sync,

  // block device configuration
  .read_size = LFS_RW_SIZE,
  .prog_size = LFS_RW_SIZE,
  .block_size = 128,
  .block_count = 256,
  .cache_size = LFS_CACHE_SIZE,
  .lookahead_size = LFS_LOOKAHEAD_SIZE,
  .block_cycles = 1000,
  
  .read_buffer = read_buffer,
  .prog_buffer = prog_buffer,
  .lookahead_buffer = lookahead_buffer,
};

int storage_init(TMTDDevice *dev)
{
  LFS_ASSERT(dev != NULL);
  lfs_cfg.context = dev;
  // mount the filesystem
  int err = lfs_mount(&main_storage, &lfs_cfg);

  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err)
  {
    err = lfs_format(&main_storage, &lfs_cfg);
    if (err)
    {
      CLOG_ERROR("lfs_format(main_storage) error %d", err);
      return err;
    }
    err = lfs_mount(&main_storage, &lfs_cfg);
    if (err)
    {
      CLOG_ERROR("lfs_mount(main_storage) error %d", err);
      return err;
    }
  }
  return 0;
}
