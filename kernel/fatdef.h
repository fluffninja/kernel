#ifndef _INC_FAT16
#define _INC_FAT16

struct fat_drive_header
{
    char   oem_name[8];
    short  bytes_per_sector;
    char   sectors_per_cluster;
    short  reserved_sector_count;
    char   fat_count;
    short  root_entry_count;
    short  filesystem_sector_count;
    char   media_descriptor;
    short  sectors_per_fat;
    short  sectors_per_track;
    short  head_count;
    short  hidden_sector_count;
};

struct fat_drive_header_extension
{
    char   extension_signature;
    int    partition_serial_number;
    char   volume_label[11];
    char   filesystem_type[8];
};

struct fat16_drive_header_extension
{
    short  hidden_sector_count_upper;
    int    filesystem_sector_count;
    char   logical_drive_number;
    char   __reserved1;
};

struct fat32_drive_header_extension
{
    short  hidden_sector_count_upper;    
    int    filesystem_sector_count;
    int    sectors_per_fat;
    short  mirror_flags;
    char   filesystem_version;
    short  root_first_cluster;
    short  filesystem_info_sector;
    short  backup_boot_sector;
    char   __reserved1[11];
    char   logical_drive_number;
    char   __reserved2;
};

struct fat32_filesystem_info_sector
{
    int     signature1;
    char    __reserved1[480];
    int     signature2;
    int     free_cluster_count;
    int     next_free_cluster;
    char    __reserved2[8];
    int     signature3;
};

const int FAT32_FILESYSTEM_INFO_SECTOR_SIGNATURE1 = 0x41615252;
const int FAT32_FILESYSTEM_INFO_SECTOR_SIGNATURE2 = 0x61417272;
const int FAT32_FILESYSTEM_INFO_SECTOR_SIGNATURE3 = 0xaa550000;

emum fat_directory_entry_attributes
{
    FDEA_READ_ONLY      = 0x01,
    FDEA_HIDDEN         = 0x02,
    FDEA_SYSTEM_FILE    = 0x04,
    FDEA_VOLUME_LABEL   = 0x08,
    FDEA_SUBDIRECTORY   = 0x10,
    FDEA_ARCHIVE        = 0x20,
};

struct fat_directory_entry
{
    char   filename[11];
    char   attributes;
    char   __reserved1[10];
    short  time;
    short  date;
    short  start_cluster;
    int    file_size;
};

#endif /* _INC_FAT16 */
