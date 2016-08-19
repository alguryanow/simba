/**
 * @file filesystems/fs.h
 *
 * @section License
 * Copyright (C) 2014-2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

#ifndef __FILESYSTEMS_FS_H__
#define __FILESYSTEMS_FS_H__

#include "simba.h"

/**
 * The offset is relative to the start of the file.
 */
#define FS_SEEK_SET 0

/**
 * The offset is relative to the current position indicator.
 */
#define FS_SEEK_CUR 1

/**
 * The offset is relative to the end of the file.
 */
#define FS_SEEK_END 2

/**
 * Open for reading.
 */
#define FS_READ             0x01

/**
 * Open for write.
 */
#define FS_WRITE            0x02

/**
 * Open for reading and writing.
 */
#define FS_RDWR             (FS_READ | FS_WRITE)

/**
 * The file position indicator shall be set to the end of the file
 * prior to each write.
 */
#define FS_APPEND           0x04

/**
 * Synchronous writes.
 */
#define FS_SYNC             0x08

/**
 * Create the file if non-existent.
 */
#define FS_CREAT            0x10

/**
 * If FS_CREAT and FS_EXCL are set, file open shall fail if the file
 * exists.
 */
#define FS_EXCL             0x20

/**
 * Truncate the file to zero length.
 */
#define FS_TRUNC            0x40

/**
 * Command callback prototype.
 *
 * @param[in] argc Number of arguements in argv.
 * @param[in] argv An array of agruments.
 * @param[in] out_p Output channel.
 * @param[in] in_p Input channel.
 * @param[in] arg_p Argument passed to the init function of given
 *                  command.
 * @param[in] call_arg_p Argument passed to the ``fs_call`` function.
 *
 * @return zero(0) or negative error code.
 */
typedef int (*fs_callback_t)(int argc,
                             const char *argv[],
                             chan_t *out_p,
                             chan_t *in_p,
                             void *arg_p,
                             void *call_arg_p);

/**
 * Parameter setter callback prototype.
 *
 * @param[out] value_p Buffer the new value should be written to.
 * @param[in] src_p Value to set as a string.
 *
 * @return zero(0) or negative error code.
 */
typedef int (*fs_parameter_set_callback_t)(void *value_p,
                                           const char *src_p);

/**
 * Parameter printer callback prototype.
 *
 * @param[in] chout_p Channel to write the formatted value to.
 * @param[in] value_p Value to format and print to the output channel.
 *
 * @return zero(0) or negative error code.
 */
typedef int (*fs_parameter_print_callback_t)(chan_t *chout_p,
                                             void *value_p);

enum fs_type_t {
    fs_type_fat16_t = 0,
    fs_type_spiffs_t
};

/* File system. */
struct fs_filesystem_t {
    const char *name_p;
    enum fs_type_t type;
    void *fs_p;
    struct fs_filesystem_t *next_p;
};

/* A file. */
struct fs_file_t {
    struct fs_filesystem_t *filesystem_p;
    union {
        struct fat16_file_t fat16;
        spiffs_file_t spiffs;
    } u;
};

/* Command. */
struct fs_command_t {
    const FAR char *path_p;
    fs_callback_t callback;
    void *arg_p;
    struct fs_command_t *next_p;
};

/* Counter. */
struct fs_counter_t {
    struct fs_command_t command;
    long long unsigned int value;
    struct fs_counter_t *next_p;
};

/* Parameter. */
struct fs_parameter_t {
    struct fs_command_t command;
    fs_parameter_set_callback_t set_cb;
    fs_parameter_print_callback_t print_cb;
    void *value_p;
    struct fs_parameter_t *next_p;
};

/**
 * Initialize the file system module. This function must be called
 * before calling any other function in this module.
 *
 * The module will only be initialized once even if this function is
 * called multiple times.
 *
 * @return zero(0) or negative error code.
 */
int fs_module_init(void);

/**
 * Call given file system command with given input and output
 * channels. Quote an argument if it contains spaces, otherwise it is
 * parsed as multiple arguments. Any quotation mark in an argument
 * string must be escaped with a backslash (``\``), otherwise it is
 * interpreted as a string quotation mask.
 *
 * @param[in,out] command_p Command string to call. The command string
 *                          will be modified by this function, so
 *                          don't use it after this function returns.
 * @param[in] chin_p Input channel.
 * @param[in] chout_p Output channel.
 * @param[in] arg_p User argument passed to the command callback
 *                  function as ``call_arg_p``.
 *
 * @return zero(0) or negative error code.
 */
int fs_call(char *command_p,
            chan_t *chin_p,
            chan_t *chout_p,
            void *arg_p);

/**
 * Open a file by file path and mode flags. File operations are
 * permitted after the file has been opened.
 *
 * The path can be either absolute or relative. It's an absolute path
 * if it starts with a forward slash ``/``, and relative
 * otherwise. Relative paths are are relative to the current working
 * directory, given by the thread environment variable ``CWD``.
 *
 * @param[out] self_p File object to be initialized.
 * @param[in] path_p Path of the file to open. The path can be
 *                   absolute or relative.
 * @param[in] flags Mode of file open. A combination of ``FS_READ``,
 *                  ``FS_RDONLY``, ``FS_WRITE``, ``FS_WRONLY``,
 *                  ``FS_RDWR``, ``FS_APPEND``, ``FS_SYNC``,
 *                  ``FS_CREAT``, ``FS_EXCL`` and ``FS_TRUNC``.
 *
 * @return zero(0) or negative error code.
 */
int fs_open(struct fs_file_t *self_p, const char *path_p, int flags);

/**
 * Close given file. No file operations are permitted on a closed
 * file.
 *
 * @param[in] self_p Initialized file object.
 *
 * @return zero(0) or negative error code.
 */
int fs_close(struct fs_file_t *self_p);

/**
 * Read from given file into given buffer.
 *
 * @param[in] self_p Initialized file object.
 * @param[out] dst_p Buffer to read data into.
 * @param[in] size Number of bytes to read.
 *
 * @return Number of bytes read or negative error code.
 */
ssize_t fs_read(struct fs_file_t *self_p, void *dst_p, size_t size);

/**
 * Read one line from given file into given buffer. The function reads
 * one character at a time from given file until the destination
 * buffer is full, a newline ``\n`` is found or end of file is
 * reached.
 *
 * @param[in] self_p Initialized file object.
 * @param[out] dst_p Buffer to read data into. Should fit the whole
 *                   line and null-termination.
 * @param[in] size Size of the destination buffer.
 *
 * @return If a line was found the number of bytes read not including
 *         the null-termination is returned. If the destination buffer
 *         becomes full before a newline character, the destination
 *         buffer size is returned. Otherwise a negative error code is
 *         returned.
 */
ssize_t fs_read_line(struct fs_file_t *self_p, void *dst_p, size_t size);

/**
 * Write from given buffer into given file.
 *
 * @param[in] self_p Initialized file object.
 * @param[in] dst_p Buffer to write.
 * @param[in] size Number of bytes to write.
 *
 * @return Number of bytes written or negative error code.
 */
ssize_t fs_write(struct fs_file_t *self_p, const void *src_p, size_t size);

/**
 * Sets the file's read/write position relative to whence.
 *
 * @param[in] self_p Initialized file object.
 * @param[in] offset New position in bytes from given whence.
 * @param[in] whence Absolute (``FS_SEEK_SET``), relative
 *                   (``FS_SEEK_CUR``) or from end (``FS_SEEK_END``).
 *
 * @return zero(0) or negative error code.
 */
int fs_seek(struct fs_file_t *self_p, int offset, int whence);

/**
 * Return current position in the file.
 *
 * @param[in] self_p Initialized file object.
 *
 * @return Current position or negative error code.
 */
ssize_t fs_tell(struct fs_file_t *self_p);

/**
 * List files and folders in given path. Optionally
 * with given filter. The list is written to the output channel.
 *
 * @param[in] path_p Directory to list.
 * @param[in] filter_p Filter out files and folders.
 * @param[in] chout_p Output chan.
 *
 * @return zero(0) or negative error code.
 */
int fs_ls(const char *path_p,
          const char *filter_p,
          chan_t *chout_p);

/**
 * List files (callbacks) and directories in given path. Optionally
 * with given filter. The list is written to the output channel.
 *
 * @param[in] path_p Directory to list.
 * @param[in] filter_p Filter out files and folders.
 * @param[in] chout_p Output chan.
 *
 * @return zero(0) or negative error code.
 */
int fs_list(const char *path_p,
            const char *filter_p,
            chan_t *chout_p);

/**
 * Auto-complete given path.
 *
 * @param[in,out] path_p Absolute or relative path to auto-complete.
 *
 * @return >=1 if completion happened. Number of autocompleted characters
 *         added to the path.
 *         0 if no completion happend,
 *         or negative error code.
 */
int fs_auto_complete(char *path_p);

/**
 * Split buffer into path and command inplace.
 *
 * @param[in] buf_p Buffer to split.
 * @param[out] path_pp Path or NULL if no path was found.
 * @param[out] cmd_pp Command or empty string.
 *
 * @return zero(0) or negative error code.
 */
void fs_split(char *buf_p, char **path_pp, char **cmd_pp);

/**
 * Merge path and command previously split using `fs_split()`.
 *
 * @param[in] path_p Path from spilt.
 * @param[in] cmd_p Command from split.
 *
 * @return zero(0) or negative error code.
 */
void fs_merge(char *path_p, char *cmd_p);

/**
 * Initialize given file system.
 *
 * @param[in] self_p File system to initialize.
 * @param[in] name_p Path to register.
 * @param[in] type File system type.
 * @param[in] fs_p File system pointer.
 *
 * @return zero(0) or negative error code.
 */
int fs_filesystem_init(struct fs_filesystem_t *self_p,
                       const char *name_p,
                       enum fs_type_t type,
                       void *fs_p);

/**
 * Register given file system. Use the functions `fs_open()`,
 * `fs_read()`, `fs_write()`, `fs_close()`, `fs_seek()`, fs_tell() and
 * `fs_read_line()` to access files in a registerd file system.
 *
 * @param[in] self_p File system to register.
 *
 * @return zero(0) or negative error code.
 */
int fs_filesystem_register(struct fs_filesystem_t *self_p);

/**
 * Deregister given file system.
 *
 * @param[in] self_p File system to deregister.
 *
 * @return zero(0) or negative error code.
 */
int fs_filesystem_deregister(struct fs_filesystem_t *self_p);

/**
 * Initialize given command.
 *
 * @param[in] self_p Command to initialize.
 * @param[in] path_p Path to register.
 * @param[in] callback Command callback function.
 * @param[in] arg_p Callback argument.
 *
 * @return zero(0) or negative error code.
 */
int fs_command_init(struct fs_command_t *self_p,
                    const FAR char *path_p,
                    fs_callback_t callback,
                    void *arg_p);

/**
 * Register given command. Registered commands are called by the
 * function `fs_call()`.
 *
 * @param[in] command_p Command to register.
 *
 * @return zero(0) or negative error code.
 */
int fs_command_register(struct fs_command_t *command_p);

/**
 * Deregister given command.
 *
 * @param[in] command_p Command to deregister.
 *
 * @return zero(0) or negative error code.
 */
int fs_command_deregister(struct fs_command_t *command_p);

/**
 * Initialize given counter.
 *
 * @param[in] self_p Counter to initialize.
 * @param[in] path_p Path to register.
 * @param[in] value Initial value of the counter.
 *
 * @return zero(0) or negative error code.
 */
int fs_counter_init(struct fs_counter_t *self_p,
                    const FAR char *path_p,
                    uint64_t value);

/**
 * Increment given counter.
 *
 * @param[in] self_p Command to initialize.
 * @param[in] value Increment value.
 *
 * @return zero(0) or negative error code.
 */
int fs_counter_increment(struct fs_counter_t *self_p,
                         uint64_t value);

/**
 * Register given counter.
 *
 * @param[in] counter_p Counter to register.
 *
 * @return zero(0) or negative error code.
 */
int fs_counter_register(struct fs_counter_t *counter_p);

/**
 * Deregister given counter.
 *
 * @param[in] counter_p Counter to deregister.
 *
 * @return zero(0) or negative error code.
 */
int fs_counter_deregister(struct fs_counter_t *counter_p);

/**
 * Initialize given parameter.
 *
 * @param[in] self_p Parameter to initialize.
 * @param[in] path_p Path to register.
 * @param[in] set_cb Callback function set set the parameter value.
 * @param[in] print_cb Callback function set print the parameter
 *                     value.
 * @param[in] value_p Value storage area.
 *
 * @return zero(0) or negative error code.
 */
int fs_parameter_init(struct fs_parameter_t *self_p,
                      const FAR char *path_p,
                      fs_parameter_set_callback_t set_cb,
                      fs_parameter_print_callback_t print_cb,
                      void *value_p);

/**
 * Register given parameter.
 *
 * @param[in] parameter_p Parameter to register.
 *
 * @return zero(0) or negative error code.
 */
int fs_parameter_register(struct fs_parameter_t *parameter_p);

/**
 * Deregister given parameter.
 *
 * @param[in] parameter_p Parameter to deregister.
 *
 * @return zero(0) or negative error code.
 */
int fs_parameter_deregister(struct fs_parameter_t *parameter_p);

/**
 * Integer parameter setter function callback 
 *
 * @param[out] value_p Buffer the new value should be written to.
 * @param[in] src_p Value to set as a string.
 *
 * @return zero(0) or negative error code.
 */
int fs_parameter_int_set(void *value_p, const char *src_p);

/**
 * Integer parameter printer function callback 
 *
 * @param[in] chout_p Channel to write the formatted value to.
 * @param[in] value_p Value to format and print to the output channel.
 *
 * @return zero(0) or negative error code.
 */
int fs_parameter_int_print(chan_t *chout_p, void *value_p);

#endif
