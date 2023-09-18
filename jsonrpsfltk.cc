/**** file guifltk-refpersys/jsonrpsfltk.cc ******
 ****  SPDX-License-Identifier: MIT ******
 *
 * © Copyright 2023 The  Reflective Persistent System Team
 * team@refpersys.org &   http://refpersys.org/
 *
 * contributors: Basile Starynkevitch <basile@starynkevitch.net>
 *
 **********************************************/

#include "fltkrps.hh"

/** important NOTICE
 *
 * Don't change the code of the below function
 * rps_compute_cstr_two_64bits_hash after mid-september 2023. This
 * code is shared (copied) into the guifltk-refpersys program file
 * jsonrpsfltk.cc from RefPerSys source code scalar_rps.cc before its
 * line 128 near RefPerSys commit d2159c536407 ...
 *
 * This rps_compute_cstr_two_64bits_hash function uses the u8_mbtouc
 * function from GNU libunistring library to handle Unicode UTF8
 * characters.
 *
 * Function: int u8_mbtouc (ucs4_t *puc, const uint8_t *s, size_t n)
 *   Returns the length (number of units) of the first character in s,
 *   putting its ucs4_t representation in *puc. Upon failure, *puc is
 *   set to 0xfffd, and an appropriate number of units is returned.
 *
 * This u8_mbtouc function fails if an invalid sequence of units is
 * encountered at the beginning of s, or if additional units (after
 * the n provided units) would be needed to form a character.
 **/
int
rps_compute_cstr_two_64bits_hash(int64_t ht[2], const char*cstr, int len)
{
    if (!ht || !cstr)
        return 0;
    if (len < 0)
        len = strlen(cstr);
    ht[0] = 0;
    ht[1] = 0;
    if (len == 0)
        return 0;
    int64_t h0=len, h1=60899;
    const char*end = cstr + len;
    int utf8cnt = 0;
    for (const char*pc = cstr; pc < end; )
        {
            ucs4_t uc1=0, uc2=0, uc3=0, uc4=0;
            int l1 = u8_mbtouc(&uc1, (const uint8_t*)pc, end - pc);
            if (l1<0)
                return 0;
            utf8cnt ++;
            pc += l1;
            if (pc >= end)
                break;
            h0 = (h0 * 60869) ^ (uc1 * 5059 + (h1 & 0xff));
            int l2 = u8_mbtouc(&uc2, (const uint8_t*)pc, end - pc);
            if (l2<0)
                return 0;
            h1 = (h1 * 53087) ^ (uc2 * 43063 + utf8cnt + (h0 & 0xff));
            utf8cnt ++;
            pc += l2;
            if (pc >= end)
                break;
            int l3 = u8_mbtouc(&uc3, (const uint8_t*)pc, end - pc);
            if (l3<0)
                return 0;
            h1 = (h1 * 73063) ^ (uc3 * 53089 + (h0 & 0xff));
            utf8cnt ++;
            pc += l3;
            if (pc >= end)
                break;
            int l4 = u8_mbtouc(&uc4, (const uint8_t*)pc, end - pc);
            if (l4<0)
                return 0;
            h0 = (h0 * 73019) ^ (uc4 * 23057 + 11 * (h1 & 0x1ff));
            utf8cnt ++;
            pc += l4;
            if (pc >= end)
                break;
        }
    ht[0] = h0;
    ht[1] = h1;
    return utf8cnt;
} // end of rps_compute_cstr_two_64bits_hash

#warning guifltk-refpersys/jsonrpsfltk.cc is almost empty should contain JSONCPP related code

void
out_fd_handler(int fd, void*data)
{
    char buf[frps_buffer_size+4];
    memset (buf, 0, sizeof(buf));
    ssize_t nb = read(fd, buf, frps_buffer_size);
    if (nb<0)
        {
        }
    else if (nb==0)
        {
            // end of file
            Fl::remove_fd(fd);
        }
    else
        {
#warning should do something with buf
        }
} // end out_fd_handler

void
cmd_fd_handler(int fd, void*data)
{
    char buf[frps_buffer_size];
    memset (buf, 0, sizeof(buf));
#warning cmd_fd_handler has to be coded
} // end cmd_fd_handler

/* TODO: add code to communicate by JSONRPC with refpersys */
