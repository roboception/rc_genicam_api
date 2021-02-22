//-----------------------------------------------------------------------------
//  (c) 2011 by Teledyne DALSA
//  Section: Digital Imaging
//  Project: GenCP
//  Author:  Eric Bourbonnais
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group. 
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'. 
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
// 
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP 
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, 
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO, 
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS; 
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY, 
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
//
//  Description:
//      This file contains the defines required by GenCP protocol
//
//////////////////////////////////////////////////////////////////////////////
#ifndef GENCP_DEFINES_H__
#define GENCP_DEFINES_H__

#if defined(WIN32)
    #include <stdlib.h>
#else
    #include <sys/types.h>
    #include <netinet/in.h>
#endif

#if defined(WIN32)
    #pragma intrinsic(_byteswap_ushort)
    #pragma intrinsic(_byteswap_ulong)
    #pragma intrinsic(_byteswap_uint64)
    #define bswap_16(s) _byteswap_ushort(s)
    #define bswap_32(s) _byteswap_ulong(s)
    #define bswap_64(s) _byteswap_uint64(s)
#else
    #define bswap_16(s) ((((s) << 8) & 0xff00U) | (((s) >> 8) & 0x00ffU))
    #define bswap_32(l) ((((l) << 24) & 0xff000000) | (((l) << 8) & 0x00ff0000) | (((l) >> 8) & 0x0000ff00) | (((l) >> 24) & 0x000000ff))
    #define bswap_64(ll) (                          \
           (((ll) << 56) & 0xff00000000000000LL) |    \
           (((ll) << 40) & 0x00ff000000000000LL) |    \
           (((ll) << 24) & 0x0000ff0000000000LL) |    \
           (((ll) <<  8) & 0x000000ff00000000LL) |    \
           (((ll) >>  8) & 0x00000000ff000000LL) |    \
           (((ll) >> 24) & 0x0000000000ff0000LL) |    \
           (((ll) >> 40) & 0x000000000000ff00LL) |    \
           (((ll) >> 56) & 0x00000000000000ffLL))
#endif

//! Macro to set a QWORD in the datagram
#define SET_QWORD( p, o, v ) \
   (*( CLINT64* )( ( (char *)(p) ) + o ) = bswap_64(v))

//! Macro to get a QWORD from the datagram
#define GET_QWORD( p, o ) \
   (bswap_64( *(( CLINT64* )( ( (char *)(p) ) + o ) ) ) )
   
//! Macro to set a DWORD in the datagram
#define SET_DWORD( p, o, v ) \
   (*( CLUINT32 * )( ( (char *)(p) ) + o ) = bswap_32(v))

//! Macro to get a DWORD from the datagram
#define GET_DWORD( p, o ) \
   (bswap_32( *( CLUINT32 * )( ( (char *)(p) ) + o ) ))

//! Macro to set a WORD in the datagram
#define SET_WORD( p, o, v ) \
   (*( UINT16 * )( ( (char *)(p) ) + o ) = bswap_16(v))

//! Macro to get a WORD from the datagram
#define GET_WORD( p, o ) \
   (bswap_16( *( UINT16 * )( ( (char *)(p) ) + o ) ))

//! Macro to set a BYTE in the datagram
#define SET_BYTE( p, o, v ) \
   (*( char * )( ( (char *)(p) ) + o ) = (v))

//! Macro to get a BYTE from the datagram
#define GET_BYTE( p, o ) \
   ( *( char * )( ( (char *)(p) ) + o ) )

//! Macro to get the address of a field in the datagram
#define GET_PTR( p, o ) \
   ( ( char * )( ( (char *)(p) ) + o ) )
      
///////////////////////////////////////////////////////////////////////////////
// Prefix

#ifdef CAMERA_LINK_HS
# define GENCP_PREFIX_PREAMBLE_SIZE      sizeof(UINT16)
# define GENCP_PREFIX_CHANNEL_ID_SIZE    sizeof(UINT16)
# define GENCP_PREFIX_SIZE               ( GENCP_PREFIX_PREAMBLE_SIZE + GENCP_PREFIX_CHANNEL_ID_SIZE )

# define GENCP_PREFIX_PREAMBLE_OFFSET    0
# define GENCP_PREFIX_CHANNEL_ID_OFFSET  ( GENCP_PREFIX_PREAMBLE_OFFSET + GENCP_PREFIX_PREAMBLE_SIZE )

# define GENCP_POSTFIX_CRC_SIZE          sizeof(UINT16)
# define GENCP_POSTFIX_RESERVED_SIZE     sizeof(UINT16)
# define GENCP_POSTFIX_SIZE              ( GENCP_POSTFIX_CRC_SIZE + GENCP_POSTFIX_RESERVED_SIZE )
# define GENCP_POSTFIX_CRC_OFFSET        0
# define GENCP_POSTFIX_RESERVED_OFFSET   ( GENCP_POSTFIX_CRC_OFFSET + GENCP_POSTFIX_CRC_SIZE )
# define GENCP_MAX_PACKET_SIZE           0xFFFF
#elif defined(CAMERA_LINK)
# define GENCP_PREFIX_PREAMBLE 0x0100
# define GENCP_PREFIX_PREAMBLE_LENGTH    sizeof(UINT16)
# define GENCP_PREFIX_HEADER_SUM_LENGTH  sizeof(UINT16)
# define GENCP_PREFIX_FRAME_SUM_LENGTH   sizeof(UINT16)
# define GENCP_PREFIX_CHANNEL_LENGTH     sizeof(UINT16)

# define GENCP_PREFIX_PREAMBLE_OFFSET    0
# define GENCP_PREFIX_HEADER_SUM_OFFSET  ( GENCP_PREFIX_PREAMBLE_OFFSET + GENCP_PREFIX_PREAMBLE_LENGTH)
# define GENCP_PREFIX_FRAME_SUM_OFFSET   ( GENCP_PREFIX_HEADER_SUM_OFFSET + GENCP_PREFIX_HEADER_SUM_LENGTH)
# define GENCP_PREFIX_CHANNEL_OFFSET     ( GENCP_PREFIX_FRAME_SUM_OFFSET + GENCP_PREFIX_FRAME_SUM_LENGTH)

# define GENCP_PREFIX_SIZE 8
# define GENCP_POSTFIX_SIZE 0
# define GENCP_MAX_PACKET_SIZE           ( (1000+8+8) + GENCP_PREFIX_SIZE + GENCP_POSTFIX_SIZE)
# define GENCP_PREFIX_CHANNEL_DEFAULT    0
#else
# define GENCP_PREFIX_SIZE               0
# define GENCP_POSTFIX_SIZE              0
# define GENCP_MAX_PACKET_SIZE           0xFFFF
#endif

#define GENCP_VERSION_ALPHA       0x00000005
#define GENCP_VERSION_RELEASE_1   0x00010000
#define GENCP_VERSION_RELEASE_1_1 0x00010001
#ifndef GENCP_VERSION_CURRENT
# define GENCP_VERSION_CURRENT  GENCP_VERSION_RELEASE_1_1
#endif

typedef UINT16   GENCP_STATUS;
///////////////////////////////////////////////////////////////////////////////
// Common Command Section Header
#define GENCP_CCD_STATUS_LENGTH    sizeof(UINT16)
#define GENCP_CCD_FLAG_LENGTH      sizeof(UINT16)
#define GENCP_CCD_COMMAND_LENGTH   sizeof(UINT16)
#define GENCP_CCD_LENGTH_LENGTH    sizeof(UINT16)
#define GENCP_CCD_REQ_ID_LENGTH    sizeof(UINT16)

#define GENCP_CCD_STATUS_OFFSET     0
#define GENCP_CCD_FLAGS_OFFSET      0
#define GENCP_CCD_COMMAND_OFFSET    ( GENCP_CCD_FLAGS_OFFSET + GENCP_CCD_FLAG_LENGTH)
#define GENCP_CCD_LENGTH_OFFSET     ( GENCP_CCD_COMMAND_OFFSET + GENCP_CCD_COMMAND_LENGTH)
#define GENCP_CCD_REQID_OFFSET      ( GENCP_CCD_LENGTH_OFFSET + GENCP_CCD_LENGTH_LENGTH)
#define GENCP_CCD_SIZE              ( GENCP_CCD_REQID_OFFSET + GENCP_CCD_REQ_ID_LENGTH)

///////////////////////////////////////////////////////////////////////////////
// Command Command Section Fields

// Flags
#define GENCP_CCD_FLAG_ACK_REQUEST      (1<<14)
#define GENCP_CCD_FLAG_RESEND           (1<<15)

// Commands
#define GENCP_CCD_COMMAND_READMEM_REQ           0x0800
#define GENCP_CCD_COMMAND_READMEM_ACK           0x0801
#define GENCP_CCD_COMMAND_WRITEMEM_REQ          0x0802
#define GENCP_CCD_COMMAND_WRITEMEM_ACK          0x0803
#define GENCP_CCD_COMMAND_PENDING_ACK           0x0805
#define GENCP_CCD_COMMAND_EVENT_REQ             0x0C00
#define GENCP_CCD_COMMAND_EVENT_ACK             0x0C01

// Status Code
#define GENCP_CCD_STATUS_SUCCESS                0x0000
#define GENCP_CCD_STATUS_NOT_IMPLEMENTED        0x8001
#define GENCP_CCD_STATUS_INVALID_PARAMETER      0x8002
#define GENCP_CCD_STATUS_INVALID_ADDRESS        0x8003
#define GENCP_CCD_STATUS_WRITE_PROTECT          0x8004
#define GENCP_CCD_STATUS_BAD_ALIGNMENT          0x8005
#define GENCP_CCD_STATUS_ACCESS_DENIED          0x8006
#define GENCP_CCD_STATUS_BUSY                   0x8007
#define GENCP_CCD_STATUS_LOCAL_PROBLEM          0x8008
#define GENCP_CCD_STATUS_MISMATCH               0x8009
#define GENCP_CCD_STATUS_INVALID_PROTOCOL       0x800A
#define GENCP_CCD_STATUS_MSG_TIMEOUT            0x800B
#define GENCP_CCD_STATUS_INVALID_HEADER         0x800E
#define GENCP_CCD_STATUS_WRONG_CONFIG           0x800F
#define GENCP_CCD_STATUS_GENERIC_ERROR          0x8FFF

#define GENCP_LIB_STATUS_SUCCESS                     0
#define GENCP_LIB_STATUS_BAD_PACKET_SIZE        -20500
#define GENCP_LIB_STATUS_BAD_DATA_PACKET_SIZE   -20501
#define GENCP_LIB_STATUS_INVALID_PACKET         -20502
#define GENCP_LIB_STATUS_INVALID_ANSWER         -20503
#define GENCP_LIB_STATUS_INVALID_REQ_ID         -20504
#define GENCP_LIB_STATUS_INSUFFICENT_RESOURCE   -20505
#define GENCP_LIB_STATUS_INCOMPATIBLE_PROTOCOL  -20506
#define GENCP_LIB_STATUS_BAD_REGISTER_SIZE      -20507
#define GENCP_LIB_STATUS_NO_TRANSACTION_PENDING -20508
#define GENCP_LIB_STATUS_UNZIPED_FAILED         -20509

///////////////////////////////////////////////////////////////////////////////
// Specific Command Section Header
#define GENCP_MAX_SCD_SIZE               ( GENCP_MAX_PACKET_SIZE - GENCP_CCD_SIZE)
#define GENCP_SCD_ADDR_SIZE              ( sizeof(CLINT64) )
#define GENCP_SCD_READMEM_RESERVE_SIZE   ( sizeof(UINT16) )
#define GENCP_SCD_READMEM_LENGTH_SIZE    ( sizeof(UINT16) )

#define GENCP_SCD_ADDR_OFFSET            ( GENCP_CCD_SIZE )
#define GENCP_SCD_READMEM_DATA_OFFSET    ( GENCP_CCD_SIZE )
#define GENCP_SCD_READMEM_RESERVE_OFFSET ( GENCP_SCD_ADDR_OFFSET + GENCP_SCD_ADDR_SIZE )
#define GENCP_SCD_READMEM_LENGTH_OFFSET  ( GENCP_SCD_READMEM_RESERVE_OFFSET + GENCP_SCD_READMEM_RESERVE_SIZE )

#define GENCP_SCD_WRITEMEM_DATA_OFFSET   ( GENCP_SCD_ADDR_OFFSET + GENCP_SCD_ADDR_SIZE )

#define GENCP_SCD_WRITEMEM_ACK_RESERVE_SIZE  ( sizeof(UINT16) )
#define GENCP_SCD_WRITEMEM_ACK_WRITTEN_SIZE  ( sizeof(UINT16) )

#define GENCP_SCD_READMEM_REQ_SIZE       ( GENCP_SCD_READMEM_LENGTH_OFFSET + GENCP_SCD_READMEM_LENGTH_SIZE )
#define GENCP_SCD_WRITEMEM_ACK_SIZE      ( GENCP_CCD_SIZE + GENCP_SCD_WRITEMEM_ACK_RESERVE_SIZE + GENCP_SCD_WRITEMEM_ACK_WRITTEN_SIZE )
#define GENCP_SCD_READMEM_MAX_DATA_SIZE  ( GENCP_MAX_SCD_SIZE )
#define GENCP_SCD_WRITEMEM_MAX_DATA_SIZE ( GENCP_MAX_SCD_SIZE - GENCP_SCD_ADDR_SIZE )

// Pending
#define GENCP_SCD_PENDING_RESERVE_SIZE   ( sizeof(UINT16) )
#define GENCP_SCD_PENDING_TIMEOUT_SIZE   ( sizeof(UINT16) )
#define GENCP_SCD_PENDING_RESERVE_OFFSET ( GENCP_CCD_SIZE )
#define GENCP_SCD_PENDING_TIMEOUT_OFFSET ( GENCP_CCD_SIZE + GENCP_SCD_PENDING_RESERVE_SIZE )
#define GENCP_SCD_PENDING_ACK_SIZE       ( GENCP_SCD_PENDING_TIMEOUT_OFFSET + GENCP_SCD_PENDING_TIMEOUT_SIZE )

// Event
#define GENCP_SCD_EVENT_RESERVED_SIZE    ( sizeof(UINT16) )
#define GENCP_SCD_EVENTID_SIZE           ( sizeof(UINT16) )
#define GENCP_SCD_TIMESTAMP_SIZE         ( sizeof(UINT64) )
#define GENCP_SCD_EVENT_EVENTID_OFFSET   ( GENCP_CCD_SIZE + GENCP_SCD_EVENT_RESERVED_SIZE)
#define GENCP_SCD_EVENT_TIMESTAMP_OFFSET ( GENCP_SCD_EVENT_EVENTID_OFFSET + GENCP_SCD_EVENTID_SIZE)
#define GENCP_SCD_EVENT_DATA_OFFSET      ( GENCP_SCD_EVENT_TIMESTAMP_OFFSET + GENCP_SCD_TIMESTAMP_SIZE)

///////////////////////////////////////////////////////////////////////////////
// Boot Strap Registers
#if GENCP_VERSION_CURRENT >= 0x00010000 
#define _GENCP_COMPATIBILITY_OFFSET   64
#else
#define _GENCP_COMPATIBILITY_OFFSET   0
#endif

#define PROTOCOL_VERSION_OFFSET              0x0000
#define MANUFACTURER_NAME_OFFSET             0x0004
#define DEVICE_MODEL_OFFSET                  0x0044
#define DEVICE_FAMILY_OFFSET                 0x0084
#define DEVICE_VERSION_OFFSET                (_GENCP_COMPATIBILITY_OFFSET + 0x0084)
#define MANUFACTURER_INFO_OFFSET             (_GENCP_COMPATIBILITY_OFFSET + 0x00C4)
#define SERIAL_NUMBER_OFFSET                 (_GENCP_COMPATIBILITY_OFFSET + 0x0104)
#define USER_DEFINED_NAME_OFFSET             (_GENCP_COMPATIBILITY_OFFSET + 0x0144)
#define DEVICE_CAPABILITY_OFFSET             (_GENCP_COMPATIBILITY_OFFSET + 0x0184)
#define MAX_DEVICE_RESPONSE_TIME_OFFSET      (_GENCP_COMPATIBILITY_OFFSET + 0x018C)
#define MANIFEST_TABLE_ADDR_OFFSET           (_GENCP_COMPATIBILITY_OFFSET + 0x0190)
#define SBRM_ADDR_OFFSET                     (_GENCP_COMPATIBILITY_OFFSET + 0x0198)
#define DEVICE_CONFIGURATION_OFFSET          (_GENCP_COMPATIBILITY_OFFSET + 0x01A0)
#define HEARTBEAT_TIMEOUT_OFFSET             (_GENCP_COMPATIBILITY_OFFSET + 0x01A8)
#define MESSAGE_CHANNEL_ID_OFFSET            (_GENCP_COMPATIBILITY_OFFSET + 0x01AC)
#define TIMESTAMP_OFFSET                     (_GENCP_COMPATIBILITY_OFFSET + 0x01B0)
#define TIMESTAMP_LATCH_OFFSET               (_GENCP_COMPATIBILITY_OFFSET + 0x01B8)
#define TIMESTAMP_INCR_OFFSET                (_GENCP_COMPATIBILITY_OFFSET + 0x01BC)
#define ACCESS_PRIVILEGE_OFFSET              (_GENCP_COMPATIBILITY_OFFSET + 0x01C4)
#define PROTOCOL_ENDIANESS_OFFSET            (_GENCP_COMPATIBILITY_OFFSET + 0x01C8)
#define IMPLEMENTATION_ENDIANESS_OFFSET      (_GENCP_COMPATIBILITY_OFFSET + 0x01CC)

#define PROTOCOL_VERSION_SIZE                4
#define MANUFACTURER_NAME_SIZE               64
#define DEVICE_MODEL_SIZE                    64
#define DEVICE_FAMILY_SIZE                   64
#define DEVICE_VERSION_SIZE                  64
#define MANUFACTURER_INFO_SIZE               64
#define SERIAL_NUMBER_SIZE                   64
#define USER_DEFINED_NAME_SIZE               64
#define DEVICE_CAPABILITY_SIZE               8
#define MAX_DEVICE_RESPONSE_TIME_SIZE        4
#define MANIFEST_TABLE_ADDR_SIZE             8
#define SBRM_ADDR_SIZE                       8
#define DEVICE_CONFIGURATION_SIZE            8
#define HEARTBEAT_TIMEOUT_SIZE               4
#define MESSAGE_CHANNEL_ID_SIZE              4
#define TIMESTAMP_SIZE                       8
#define TIMESTAMP_LATCH_SIZE                 4
#define TIMESTAMP_INCR_SIZE                  8
#define ACCESS_PRIVILEGE_SIZE                4
#define PROTOCOL_ENDIANESS_SIZE              4
#define IMPLEMENTATION_ENDIANESS_SIZE        4

#define CAPABILITY_USER_NAME_AVAILABLE              0x0000000000000001ull
#define CAPABILITY_ACCESS_PRIVILEGE_AVAILABLE       0x0000000000000002ull
#define CAPABILITY_MESSAGE_CHANNEL_SUPPORTED        0x0000000000000004ull
#define CAPABILITY_TIMESTAMP_SUPPORTED              0x0000000000000008ull
#define CAPABILITY_STRING_ENCODING                  0x00000000000000F0ull
#define CAPABILITY_FAMILY_REGISTER_AVAILABLE        0x0000000000000100ull
#define CAPABILITY_SBRM_SUPPORTED                   0x0000000000000200ull
#define CAPABILITY_ENDIANESS_REGISTER_SUPPORTED     0x0000000000000400ull
#define CAPABILITY_WRITTEN_LENGTH_SUPPORTED         0x0000000000000800ull
#define CAPABILITY_MULTIEVENT_SUPPORTED             0x0000000000001000ull

#define CONFIGURATION_HEARTBEAT_ENABLE              0x0000000000000001ull
#define CONFIGURATION_MULTIEVENT_ENABLE             0x0000000000000002ull

#define ACCESS_PRIVILEGE_MASK                       0x00000007l
#define ACCESS_PRIVILEGE_AVAILABLE                  0
#define ACCESS_PRIVILEGE_EXCLUSIVE                  1

#define CL_SERIAL_BITRATEX_AVAILABLE_OFFSET  ( SBRM_OFFSET )
#define CL_SERIAL_BITRATE_AVAILABLE          4
#define CL_SERIAL_BITRATEX_OFFSET            ( CL_SERIAL_BITRATEX_AVAILABLE_OFFSET + CL_SERIAL_BITRATE_AVAILABLE )
#define CL_SERIAL_BITRATE_SIZE               4

#define MANIFEST_ENTRY_FILE_INFO_INDEX              0
#define MANIFEST_ENTRY_FILE_VERSION_MAJOR_INDEX     0
#define MANIFEST_ENTRY_FILE_VERSION_MINOR_INDEX     1
#define MANIFEST_ENTRY_FILE_VERSION_SUBMINOR_INDEX  2
#define MANIFEST_ENTRY_FILE_GEN_VERSION_MAJOR_INDEX 4
#define MANIFEST_ENTRY_FILE_GEN_VERSION_MINOR_INDEX 5
#define MANIFEST_ENTRY_FILE_TYPE_INDEX              6
#define MANIFEST_ENTRY_FILE_ADDR_INDEX              8
#define MANIFEST_ENTRY_FILE_SIZE_INDEX              16
#define MANIFEST_ENTRY_RESERVED_INDEX               24

#define MANIFEST_ENTRY_COUNT_OFFSET           MANIFEST_OFFSET
#define MANIFEST_ENTRY_COUNT_SIZE             8
#define MANIFEST_ENTRY_SIZE                   24
#define MANIFEST_ENTRY_MAX_SIZE_SIZE          64
#define MANIFEST_ENTRYX_OFFSET(X)             ( MANIFEST_ENTRY_COUNT_OFFSET + MANIFEST_ENTRY_COUNT_SIZE + ( X * MANIFEST_ENTRY_MAX_SIZE_SIZE ) )
#define MANIFEST_ENTRYX_INFO_OFFSET(X)        ( MANIFEST_ENTRYX_OFFSET(X) + MANIFEST_ENTRY_FILE_INFO_INDEX )
#define MANIFEST_ENTRYX_FILE_ADDR_OFFSET(X)   ( MANIFEST_ENTRYX_OFFSET(X) + MANIFEST_ENTRY_FILE_ADDR_INDEX )
#define MANIFEST_ENTRYX_FILE_SIZE_OFFSET(X)   ( MANIFEST_ENTRYX_OFFSET(X) + MANIFEST_ENTRY_FILE_SIZE_INDEX )
#define MANIFEST_ENTRYX_RESERVED_OFFSET(X)    ( MANIFEST_ENTRYX_OFFSET(X) + MANIFEST_ENTRY_RESERVED_INDEX )
//#define MANIFEST_URL_SIZE                      256

#endif //GENCP_DEFINES_H__
