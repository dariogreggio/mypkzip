/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Dario's Automation

  @File Name
    pkzipgd.h

 * portions nerd_di_merd miniz library ;)
 */
/* ************************************************************************** */

#ifndef _PKZIPGD_H
#define _PKZIPGD_H

#ifdef __cplusplus
extern "C" {
#endif

struct __attribute__((__packed__)) ZIP_HEADER {
  uint32_t signature;
  uint16_t version;
  uint16_t flags;
  uint16_t compression;
  uint16_t mod_time;
  uint16_t mod_date;
  uint32_t crc32;
  uint32_t compressedSize;
  uint32_t uncompressedSize;
  uint16_t filename_len;
  uint16_t extrafield_len;
  // filename (variable
  // extra field (variable
  } header;



/* Return status. */
typedef enum __attribute__((__packed__)) {
    /* This flag indicates the inflator needs 1 or more input bytes to make forward progress, but the caller is indicating that no more are available. The compressed data */
    /* is probably corrupted. If you call the inflator again with more bytes it'll try to continue processing the input but this is a BAD sign (either the data is corrupted or you called it incorrectly). */
    /* If you call it again with no input you'll just get TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS again. */
    TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS = -4,

    /* This flag indicates that one or more of the input parameters was obviously bogus.
      (You can try calling it again, but if you get this error the calling code is wrong.) */
    TINFL_STATUS_BAD_PARAM = -3,

    /* This flag indicate the inflator is finished but the adler32 check of the uncompressed data didn't match.
      If you call it again it'll return TINFL_STATUS_DONE. */
    TINFL_STATUS_ADLER32_MISMATCH = -2,

    /* This flag indicate the inflator has somehow failed (bad code, corrupted input, etc.). 
       If you call it again without resetting via tinfl_init() it it'll just keep on returning the same status failure code. */
    TINFL_STATUS_FAILED = -1,

    /* Any status code less than TINFL_STATUS_DONE must indicate a failure. */

    /* This flag indicates the inflator has returned every byte of uncompressed data that it can, 
       has consumed every byte that it needed, has successfully reached the end of the deflate stream, and 
       if zlib headers and adler32 checking enabled that it has successfully checked the uncompressed data's adler32.
       If you call it again you'll just get TINFL_STATUS_DONE over and over again. */
    TINFL_STATUS_DONE = 0,

    /* This flag indicates the inflator MUST have more input data (even 1 byte) before it can make 
       any more forward progress, or you need to clear the TINFL_FLAG_HAS_MORE_INPUT
       flag on the next call if you don't have any more source data. If the source data was somehow corrupted 
       it's also possible (but unlikely) for the inflator to keep on demanding input to 
       proceed, so be sure to properly set the TINFL_FLAG_HAS_MORE_INPUT flag. */
    TINFL_STATUS_NEEDS_MORE_INPUT = 1,

    /* This flag indicates the inflator definitely has 1 or more bytes of uncompressed data available,
       but it cannot write this data into the output buffer.
       Note if the source compressed data was corrupted it's possible for the inflator to 
       return a lot of uncompressed data to the caller. I've been assuming you know how much 
       uncompressed data to expect 
       (either exact or worst case) and will stop calling the inflator and fail after receiving too much. 
       In pure streaming scenarios where you have no idea how many bytes to expect this may not be possible 
       so I may need to add some code to address this. */
    TINFL_STATUS_HAS_MORE_OUTPUT = 2
} tinfl_status;

/* Decompression flags used by tinfl_decompress(). */
/* TINFL_FLAG_PARSE_ZLIB_HEADER: If set, the input has a valid zlib header and ends with an adler32 checksum
   (it's a valid zlib stream). Otherwise, the input is a raw deflate stream. */
/* TINFL_FLAG_HAS_MORE_INPUT: If set, there are more input bytes available beyond the end of the supplied 
   input buffer. If clear, the input buffer contains all remaining input. */
/* TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF: If set, the output buffer is large enough to hold 
   the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB). */
/* TINFL_FLAG_COMPUTE_ADLER32: Force adler-32 checksum computation of the decompressed bytes. */
enum __attribute__((__packed__)) {
    TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
    TINFL_FLAG_HAS_MORE_INPUT = 2,
    TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
    TINFL_FLAG_COMPUTE_ADLER32 = 8
};

/* Initializes the decompressor to its initial state. */
#define tinfl_init(r)     \
    do {                  \
        (r)->m_state = 0; \
    } while(0)
#define tinfl_get_adler32(r) (r)->m_check_adler32
  

/* Max size of LZ dictionary. */
#define TINFL_LZ_DICT_SIZE 1024  //32768

typedef uint32_t tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (32)
  
enum __attribute__((__packed__)) {
    TINFL_MAX_HUFF_TABLES = 3,
    TINFL_MAX_HUFF_SYMBOLS_0 = 288,
    TINFL_MAX_HUFF_SYMBOLS_1 = 32,
    TINFL_MAX_HUFF_SYMBOLS_2 = 19,
    TINFL_FAST_LOOKUP_BITS = 4 /*10*/,        // dice di mettere 1 per ridurre uso memoria
    TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS
};
struct __attribute__((__packed__)) tinfl_decompressor_tag {
    uint32_t m_state, m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final,
        m_type, m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
    tinfl_bit_buf_t m_bit_buf;
    size_t m_dist_from_out_buf_start;
    int16_t m_look_up[TINFL_MAX_HUFF_TABLES][TINFL_FAST_LOOKUP_SIZE];
    int16_t m_tree_0[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
    int16_t m_tree_1[TINFL_MAX_HUFF_SYMBOLS_1 * 2];
    int16_t m_tree_2[TINFL_MAX_HUFF_SYMBOLS_2 * 2];
    uint8_t m_code_size_0[TINFL_MAX_HUFF_SYMBOLS_0];
    uint8_t m_code_size_1[TINFL_MAX_HUFF_SYMBOLS_1];
    uint8_t m_code_size_2[TINFL_MAX_HUFF_SYMBOLS_2];
    uint8_t m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
};
typedef struct tinfl_decompressor_tag tinfl_decompressor;


int unzip(const char *filename,const char *dest,uint8_t flags);
int zip(const char *filename,const char *sources,uint8_t flags);

uint32_t crc32(uint8_t ch, uint32_t crc);
uint32_t crc32array(uint8_t *addr, int num, uint32_t crc);
uint32_t crc32arrayfast(uint8_t *addr, int num, uint32_t crc);
uint32_t crc32arrayHW(uint8_t *addr, int num, uint32_t crc);

    
#ifdef __cplusplus
}
#endif

#endif 
