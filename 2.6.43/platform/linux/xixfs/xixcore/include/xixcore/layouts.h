#ifndef __XIXCORE_LAYOUTS__H__
#define __XIXCORE_LAYOUTS__H__

#include "xixcore/xctypes.h"


/*
	Disk Information stored based on little endian
	
	Please check endian....
*/

/*
 *		Vitual Cluster Numbering
 */

#define CLUSTER_SHIFT							(12)
#define CLUSTER_SIZE							(1<<(CLUSTER_SHIFT))
#define CLUSTER_ALIGNED_ADDRESS(y)				( ( (y) >> (CLUSTER_SHIFT) ) << (CLUSTER_SHIFT) )
#define CLUSTER_ALIGNED_SIZE(y)					( ( ( (y) + (CLUSTER_SIZE) - 1 ) >> (CLUSTER_SHIFT) ) << (CLUSTER_SHIFT) )
#define CLUSTER_NUMBER(y)						(	(y) >> (CLUSTER_SHIFT) )
 /*
 *		Sector based calcuration
 */

#define SECTORSIZE_ALIGNED(x)					( 1 << (x))
#define SECTOR_ALIGNED_ADDR(x, y)				(((y) >> (x)) << (x))
#define SECTOR_ALIGNED_COUNT(x, y)				(  ((y) + (SECTORSIZE_ALIGNED((x)) -1)) >> (x)  )  
#define SECTOR_ALIGNED_SIZE(x, y)			  	( SECTOR_ALIGNED_COUNT((x), (y)) << (x) )	
#define IS_SECTOR_ALIGNED(x, y)					( !((y) & (SECTORSIZE_ALIGNED(x) -1) ) 

#define SECTORSHIFT_512							(9)
#define SECTORSIZE_512							( 1<<(SECTORSHIFT_512))
#define SECTORALIGNADDRESS_512(x)				(( (x) >> (SECTORSHIFT_512)) << (SECTORSHIFT_512))						
#define SECTORCOUNT_512(x)						(((x) + (SECTORSIZE_512 -1)) >> (SECTORSHIFT_512))
#define SECTORALIGNSIZE_512(x)					( SECTORCOUNT_512((x)) << (SECTORSHIFT_512))
#define IS_512_SECTOR(x)						( !((x) & 0x1FF))



#define SECTORSHIFT_4096						(12)
#define SECTORSIZE_4096							( 1<<(SECTORSHIFT_4096))
#define SECTORALIGNADDRESS_4096(x)				(( (x) >> (SECTORSHIFT_4096)) << (SECTORSHIFT_4096))						
#define SECTORCOUNT_4096(x)						(((x) + (SECTORSIZE_4096 -1)) >> (SECTORSHIFT_4096))
#define SECTORALIGNSIZE_4096(x)					( SECTORCOUNT_4096((x))<<(SECTORSHIFT_4096))
#define IS_4096_SECTOR(x)						( !((x) & 0x0FFF))



#define XIFSD_MAX_SYSTEM_RESERVED_LOT			(20)

				
#define XIFSD_READ_AHEAD_GRANULARITY			(0x10000)
#define XIFSD_MAX_HOST							(64)




// sector offset for volume record
#define XIFS_OFFSET_VOLUME_LOT_LOC				(0x00000800*(SECTORSIZE_512))
#define XIFS_VOLUME_SIGNATURE					(0x1978197306092102LL)
#define XIFS_CURRENT_VERSION					(0x1)
#define XIFS_CURRENT_RELEASE_VERSION			(0x0)
#define XIFS_MAX_FILE_NAME_LENGTH				(1000)

#define	XIFS_MAX_NAME_LEN						(2000)
#define XIFS_RESERVED_LOT_SIZE					(24)
#define XIFS_DEFAULT_USER						(8)
#define XIFS_DEFAULT_HOST_LOT_COUNT				(10)
#define	XIFS_MAX_XILOT_NUMBER					(1024*16)

/*
/
/	VOLUME INFORMATION
/		Host Register LotMap Index --> 
/			Information for Host which mount this volume is recorded in this Lot
/				- Check Out Lot Map information
/				- Additional information 
/
*/

#include "xcsystem/pshpack1.h"

#define MAX_VOLUME_LAVEL			(128)
typedef  struct _XIDISK_VOLUME_INFO{
	xc_uint64		VolumeSignature;			//8
	xc_uint32		XifsVesion;					//12
	// changed by ILGU HONG
	//		chesung suggest

	xc_uint32		LotSize;					// 16
	xc_uint64		NumLots;					// 24
	xc_uint64		HostRegLotMapIndex;			//32
	xc_uint64		RootDirectoryLotIndex;		//40
	xc_uint64		VolCreationTime;			//48
	xc_uint32		VolSerialNumber;			//52
	xc_uint32		VolLabelLength;				//56
	// CHANGED by ILGU HONG

	xc_uint8		VolumeId[16];					//72
	xc_uint8		VolLabel[128];				//200
	xc_uint32		LotSignature ;				//204
	// changed by ILGU HONG
	//		chesung suggest
	xc_uint32		_alignment_1_;				//208
	xc_uint8		Reserved[304];				//512
	xc_uint8		Reserved2[3560];			//4072
	xc_uint64		SequenceNum;				//4080
	xc_uint8		Digest[16];					//4096
}  XIDISK_VOLUME_INFO, *PXIDISK_VOLUME_INFO;


XC_CASSERT_SIZEOF(XIDISK_VOLUME_INFO,4096);

#define XIDISK_VOLUME_INFO_SIZE		(SECTORALIGNSIZE_4096(sizeof(XIDISK_VOLUME_INFO))) 


typedef struct _PACKED_BOOT_SECTOR {
    xc_uint8	Jump[3];                                //  offset = 0
    xc_uint8	Oem[8];                                 //  offset = 3
	xc_uint8	reserved;								//	offset = 11
	xc_uint8	reserved1[4];							//	offset = 12
	xc_uint64	VolumeSignature;						//	offset = 16
    xc_uint64	NumLots;								//  offset = 24
	xc_uint32	LotSignature ;							//	offset = 32
	xc_uint32	XifsVesion;								//	offset = 36
	xc_uint32	LotSize;								//	offset = 40
	xc_uint32	_alignment_1_;							//	offset = 44
	xc_uint64	FirstVolumeIndex;						//	offset = 48
	xc_uint64	SecondVolumeIndex;						//	offset = 56
    xc_uint8	reserved2[424];							//	offset = 64
	xc_uint64	SequenceNum;							//	offset = 488
	xc_uint8	Digest[16];								//	offset = 496
} PACKED_BOOT_SECTOR;                               //  sizeof = 512
typedef PACKED_BOOT_SECTOR *PPACKED_BOOT_SECTOR;






/*
/
/
/	LOT MAP 
/		Lot --> A unique data struct for xifs system
/		Threre are several Map 
/			--> 1. Used Lot Map (dirty)
/			--> 2. Unused Lot Map (free)
/			--> 3. Check out Lot Map (reserved for host)
/
/
*/
#define		XIFS_MAP_INVALID		(0)
#define		XIFS_MAP_USEDLOT		(1)
#define		XIFS_MAP_UNUSEDLOT		(2)
#define		XIFS_MAP_CHECKOUTLOT	(3)
#define		XIFS_MAP_SYSTEM			(4)
typedef  struct _XIDISK_LOT_MAP_INFO{
	xc_uint32		MapType;				//allocated/free/checkout/system	//4
	xc_uint32		BitMapBytes;												//8
	xc_uint64		NumLots;													//16
	xc_uint8		Reserved[496];			//512
	xc_uint8		Reserved2[3560];		//4072
	xc_uint64		SequenceNum;			//4080
	xc_uint8		Digest[16];				//4096
}  XIDISK_LOT_MAP_INFO, *PXIDISK_LOT_MAP_INFO;

XC_CASSERT_SIZEOF(XIDISK_LOT_MAP_INFO,4096);

#define XIDISK_LOT_MAP_INFO_SIZE		(SECTORALIGNSIZE_4096(sizeof(XIDISK_LOT_MAP_INFO))) 


/*
	BitMapDataInfo;
*/
typedef struct _XIDISK_BITMAP_DATA{
	xc_uint8		Digest[16];				//16
	xc_uint64		SequenceNum;			//24
	xc_uint8		RealData[1];
}XIDISK_BITMAP_DATA, *PXIDISK_BITMAP_DATA;

/*
/
/
/	Host Record	(512 byte aligne)
/		Stored in Host register Lot information
/		Information --> 1. Host Check out Lot Map Index
/						2. Host Used Lot Map Index
/						3. Host Unused Lot Map Index
/
/
*/
#define HOST_UMOUNT		(0x00000000)
#define HOST_MOUNTING	(0x00000001)
#define HOST_MOUNT		(0x00000002)

typedef  struct _XIDISK_HOST_RECORD {
	// changed by ILGU HONG
	//	chesung suggest
	xc_uint32		HostState;						//4
	xc_uint32		_alignment_1_;					//8
	xc_uint8		HostSignature[16];				//24
	xc_uint64		HostMountTime;					//32
	xc_uint64		HostCheckOutLotMapIndex;		//40
	xc_uint64		HostUsedLotMapIndex;			//48
	xc_uint64		HostUnusedLotMapIndex;			//56
	xc_uint64		LogFileIndex;					//64
	xc_uint8		Reserved[448];					//512
	xc_uint8		Reserved2[3560];				//4072
	xc_uint64		SequenceNum;					//4080
	xc_uint8		Digest[16];						//4096
}  XIDISK_HOST_RECORD, *PXIDISK_HOST_RECORD;

XC_CASSERT_SIZEOF(XIDISK_HOST_RECORD,4096);

#define XIDISK_HOST_RECORD_SIZE		(SECTORALIGNSIZE_4096(sizeof(XIDISK_HOST_RECORD))) 

/*
/
/		Host Info (512 byte aligne)
/			- Header of Host Register Lot
/			- Used Lot Map Index
/			- Unused Lot Map Index
/			- Check out Lot Map Index
/
/
*/
typedef  struct _XIDISK_HOST_INFO{
	// changed by ILGU HONG
	//	suggest by chesung
	xc_uint32		NumHost;						//4
	xc_uint32		_alignment_1_;					//8
	xc_uint64		UsedLotMapIndex;				//16
	xc_uint64		UnusedLotMapIndex;				//24
	xc_uint64		CheckOutLotMapIndex;			//32
	xc_uint64		LogFileIndex;					//40
	xc_uint8		RegisterMap[16];				//56
	xc_uint8		Reserved[456];					//512
	xc_uint8		Reserved2[3560];				//4072
	xc_uint64		SequenceNum;					//4080
	xc_uint8		Digest[16];						//4096
} XIDISK_HOST_INFO, *PXIDISK_HOST_INFO;

XC_CASSERT_SIZEOF(XIDISK_HOST_INFO,4096);

#define XIDISK_HOST_INFO_SIZE		(SECTORALIGNSIZE_4096(sizeof(XIDISK_HOST_INFO))) 

/*
/
/
/	Lot Lock (512 Byte aligne)
/		--> Used file lock / direcotry lock
/
/
*/

#define XIDISK_LOCK_RELEASED			(0x00000000)
#define XIDISK_LOCK_ACQUIRED			(0x00000001)

typedef  struct _XIDISK_LOCK {
	// Chagned by ILGU HONG
	// chesung suggest 
	xc_uint32	LockState;					//	4		//file/dir/valid/invalid
	xc_uint32  _alignment_1_;				//	8
	xc_uint8	LockHostSignature[16];		//	24
	xc_uint64	LockAcquireTime;			//	32
	xc_uint8	LockHostMacAddress[32];		//	64			
	xc_uint8	Reserved[448];				//	512
}  XIDISK_LOCK, *PXIDISK_LOCK;

XC_CASSERT_SIZEOF(XIDISK_LOCK,512);

#define XIDISK_LOCK_SIZE		(SECTORALIGNSIZE_512(sizeof(XIDISK_LOCK)))

/*
/
/
/	Lot Info	(512 Byte aligned)
/		Type -->		1.INVALID			(0x00000000)
/					2.VOLUME			(0x00000001)
/					3.FILE				(0x00000002)
/					4.DIRECTORY			(0x00000004)
/					5.HOSTREG			(0x00000008)
/					6.SYSTEM_MAP		(0x00000010)
/					7.BITMAP				(0x00000020)
/
/		Flags -->	1. Invalid
/					1. Begin
/					2. Body
/					3. End
/					4. unique
/
*/


/*
	LOT INFORMATION
*/
#define LOT_INFO_TYPE_INVALID				(0x00000000)
#define LOT_INFO_TYPE_VOLUME				(0x00000001)
#define LOT_INFO_TYPE_FILE					(0x00000002)
#define LOT_INFO_TYPE_DIRECTORY			(0x00000004)
#define LOT_INFO_TYPE_HOSTREG				(0x00000008)
#define LOT_INFO_TYPE_BITMAP				(0x00000010)
#define LOT_INFO_TYPE_ADDRMAP				(0x00000020)
#define LOT_INFO_TYPE_LOG					(0x00000040)



#define LOT_FLAG_INVALID					(0x00000000)
#define LOT_FLAG_BEGIN						(0x00000001)
#define LOT_FLAG_END						(0x00000002)
#define LOT_FLAG_BODY						(0x00000004)

typedef  struct _XIDISK_LOT_INFO {
	xc_uint32		Type;						//	4  - dir/file/root/map/host register
	xc_uint32		Flags;						//	8	//begin/end/body
	xc_uint64		LotIndex;					//	16
	xc_uint64		BeginningLotIndex;			//	24
	xc_uint64		PreviousLotIndex;			//	32
	xc_uint64		NextLotIndex;				//	40
	xc_uint64		LogicalStartOffset;			//	48
	xc_uint32		StartDataOffset;			//	52
	xc_uint32		LotTotalDataSize;			//	56
	xc_uint32		LotSignature;				//	60
	// changed by ILGU HONG
	//	chesung suggest
	xc_uint32		_alignment_1_;				//  64
	xc_uint8		Reserved[448];				//	512
}  XIDISK_LOT_INFO, *PXIDISK_LOT_INFO;

XC_CASSERT_SIZEOF(XIDISK_LOT_INFO,512);

#define XIDISK_LOT_INFO_SIZE	(SECTORALIGNSIZE_512(sizeof(XIDISK_LOT_INFO)))


/*
	Address Information
		required for random access 
		is designed for (1024*16 entry)

		Address Ma index
		|VALIDITY(1)|Address(63)|
		
*/
#define	XIDISK_ADDRMAP_VALID			(0x8000000000000000)
#define	XIDISK_ADDRMAP_INVALID			(0x0000000000000000)
#define	XIDISK_ADDRMAP_VALIDITY_MASK	(0xF000000000000000)

typedef  struct _XIDISK_ADDR_MAP {
	xc_uint64		LotNumber[XIFS_MAX_XILOT_NUMBER];
}  XIDISK_ADDR_MAP, *PXIDISK_ADDR_MAP;

XC_CASSERT_SIZEOF(XIDISK_ADDR_MAP,131072);

#define	XIDISK_ADDR_MAP_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_ADDR_MAP)))

#define XIDISK_ADDR_MAP_SEC_512_COUNT	((xc_uint32)(XIDISK_ADDR_MAP_SIZE/SECTORSIZE_512))
#define XIDISK_ADDR_MAP_SEC_4096_COUNT	((xc_uint32)(XIDISK_ADDR_MAP_SIZE/SECTORSIZE_4096))
#define XIDISK_ADDR_MAP_INDEX_COUNT(x)	((xc_uint32)(XIDISK_ADDR_MAP_SIZE/	(x)	))


/*
/
/
/	Dir Info	(512 Btye aligned --> 5*5*512)
/		State	1. DELETED
/				2. CREATE
/
/		Type	1.INVALID				(0x00000000)
/				2.FILE					(0x00000010)
/				3.DIRECTORY				(0x00000020)
/				4.ROOT_DIRECTORY		(0x00000040)
/				5.PAGE_FILE				(0x00000080)
/				6.SYSTEM_FILE			(0x00000100)
/				7.PIPE_FILE				(0x00000200)
/				8.DEVICE_FILE			(0x00000400)
/				9.SF_LINK				(0x00000004)
/				10.HD_LINK				(0x00000008)
/
/
/		
/
*/


/*
	DISK FILE ENTRY INFORMATION
*/
#define XIFS_FD_STATE_DELETED				(0x00000000)
#define XIFS_FD_STATE_CREATE				(0x00000001)

#define XIFS_FD_ACCESS_U_R					(0x00000001)
#define XIFS_FD_ACCESS_U_W				(0x00000002)
#define XIFS_FD_ACCESS_U_X					(0x00000004)				
#define XIFS_FD_ACCESS_G_R					(0x00000010)
#define XIFS_FD_ACCESS_G_W				(0x00000020)
#define XIFS_FD_ACCESS_G_X					(0x00000040)				
#define XIFS_FD_ACCESS_O_R					(0x00000100)				
#define XIFS_FD_ACCESS_O_W				(0x00000200)				
#define XIFS_FD_ACCESS_O_X					(0x00000400)				
#define XIFS_FD_DEFAULT_ACCESS			(0x00000557)

#define XIFS_FD_TYPE_INVALID				(0x00000000)
#define XIFS_FD_TYPE_FILE					(0x00000001)
#define XIFS_FD_TYPE_DIRECTORY				(0x00000002)
#define XIFS_FD_TYPE_ROOT_DIRECTORY		(0x00000004)
#define XIFS_FD_TYPE_PAGE_FILE				(0x00000008)
#define XIFS_FD_TYPE_SYSTEM_FILE			(0x00000010)
#define XIFS_FD_TYPE_PIPE_FILE				(0x00000020)
#define XIFS_FD_TYPE_DEVICE_FILE			(0x00000040)
#define XIFS_FD_TYPE_VOLUME				(0x00000080)
#define XIFS_FD_TYPE_CHILD_SF_LINK		(0x00000100)
#define XIFS_FD_TYPE_CHILD_HD_LINK		(0x00000200)



typedef  struct _XIDISK_DIR_INFO {
	// Changed by ILGU HONG
	//	suggested by chesung
	xc_uint32	State;						//	4	
	xc_uint32	Type;						//	8
	xc_uint32	FileAttribute;				//	12
	xc_uint32 	LinkCount;					//	16
	xc_uint32 	AccessFlags;				// 	20
	xc_uint32	ACLState;					// 	24
	xc_uint32	NameSize;					//	28
	xc_uint32	_alignment_1_;				//	32
	xc_uint64 	ParentDirLotIndex;			//	40
	xc_uint64	FileSize;					//	48	
	xc_uint64	AllocationSize;				//	56
	xc_uint64	LotIndex;					//	64
	xc_uint64	AddressMapIndex;			//	72
	xc_uint64 	Create_time;				// 	80	
	xc_uint64 	Change_time;				//	88	
	xc_uint64	Modified_time;				// 	96
	xc_uint64 	Access_time;				// 	104
	xc_uint8	OwnHostId[16];				//	120
	xc_uint32 	childCount;					//	124
	xc_uint32	_alignment_2_;				//	128
	xc_uint8	ChildMap[128];				//	256
	xc_uint8	Reserved1[256];				// 	512
	xc_uint8 	Name[2048];					//	2560	(512*5)
	xc_uint8	Reserved2[1512];			//	4072
	xc_uint64	SequenceNum;				//	4080
	xc_uint8	Digest[16];					//	4096	(512*8)
}  XIDISK_DIR_INFO, *PXIDISK_DIR_INFO;


XC_CASSERT_SIZEOF(XIDISK_DIR_INFO,4096);

#define XIDISK_DIR_INFO_SIZE	(SECTORALIGNSIZE_4096(sizeof(XIDISK_DIR_INFO)))



// Added by ILGU HONG 12082006
/*
/
/	Directroy entry hash value table
/		used for fast searching.
/			- hash value of directory entry string
/
/
*/
#define MAX_DIR_ENTRY		1024
typedef   struct _XIDISK_HASH_VALUE_TABLE{
	xc_uint16		EntryHashVal[MAX_DIR_ENTRY];//	2048
	xc_uint8		Reserved[2024];				//	4072
	xc_uint64		SequenceNum;				//	4080
	xc_uint8		Digest[16];					//	4096	(512*8)
} XIDISK_HASH_VALUE_TABLE, *PXIDISK_HASH_VALUE_TABLE;


XC_CASSERT_SIZEOF(XIDISK_HASH_VALUE_TABLE,4096);

#define XIDISK_HASH_VALUE_TABLE_SIZE	(SECTORALIGNSIZE_4096(sizeof(XIDISK_HASH_VALUE_TABLE)))
// Added by ILGU HONG 12082006 END

/*
/
/
/	Directory Link Information	(512 Byte aligned  4*512)
/		--> Used for Dir information
/
/		Type	1.INVALID				(0x00000000)
/				2.FILE					(0x00000010)
/				3.DIRECTORY				(0x00000020)
/				4.ROOT_DIRECTORY		(0x00000040)
/				5.PAGE_FILE				(0x00000080)
/				6.SYSTEM_FILE			(0x00000100)
/				7.PIPE_FILE				(0x00000200)
/				8.DEVICE_FILE			(0x00000400)
/				9.SF_LINK				(0x00000004)
/				10.HD_LINK				(0x00000008)
/
/		State	1. DELETED
/				2. CREATE
/
/
*/
typedef  struct _XIDISK_CHILD_INFORMATION{
	// Changed by ILGU HONG
	//	chesung suggest
	xc_uint32		Type;						//	4
	xc_uint32		State;						//	8 -delete/create/linked/...
	xc_uint32		NameSize;					//	12
	xc_uint32		ChildIndex;					//	16
	xc_uint64		StartLotIndex;				//	24
	xc_uint8		HostSignature[16];			//	40
	xc_uint8 		Name[2048];					//	2088
	xc_uint32		HashValue;					//	2092
	xc_uint32		_alignment_1_;				//	2096
	xc_uint8		Reserved[1976];				//	4072
	xc_uint64		SequenceNum;				//	4080
	xc_uint8		Digest[16];					//	4096	(512*8)
} XIDISK_CHILD_INFORMATION, *PXIDISK_CHILD_INFORMATION;

XC_CASSERT_SIZEOF(XIDISK_CHILD_INFORMATION,4096);

#define XIDISK_CHILD_RECORD_SIZE	(SECTORALIGNSIZE_4096(sizeof(XIDISK_CHILD_INFORMATION)))
#define XIDISK_DUP_CHILD_RECORD_SIZE	(SECTORALIGNSIZE_4096(sizeof(XIDISK_CHILD_INFORMATION) *2))
// CHANGED by ILGU HONG 12082006
//#define GetChildOffset(ChildIndex)		((ChildIndex)*XIDISK_CHILD_RECORD_SIZE)
#define GetChildOffset(ChildIndex)		( ( (ChildIndex) + 1 )*XIDISK_DUP_CHILD_RECORD_SIZE )
// CHANGED by ILGU HONG 12082006 END
#define	XILOT		ULONGLONG



/*
/
/	File Info	(512 Btye aligned --> 5*5*512)
/		State	1. DELETED
/				2. CREATE
/
/		Type	1.INVALID				(0x00000000)
/				2.FILE					(0x00000010)
/				3.DIRECTORY				(0x00000020)
/				4.ROOT_DIRECTORY		(0x00000040)
/				5.PAGE_FILE				(0x00000080)
/				6.SYSTEM_FILE			(0x00000100)
/				7.PIPE_FILE				(0x00000200)
/				8.DEVICE_FILE			(0x00000400)
/				9.SF_LINK				(0x00000004)
/				10.HD_LINK				(0x00000008)
/
*/
typedef  struct _XIDISK_FILE_INFO {
	// Changed by ILGU HONG
	//	chesung suggest
	xc_uint32	State;						//	4	
	xc_uint32	Type;						//	8
	xc_uint32	FileAttribute;				//	12
	xc_uint32 	LinkCount;					//	16
	xc_uint32 	AccessFlags;				// 	20
	xc_uint32	ACLState;					// 	24
	xc_uint32	NameSize;					//	28
	xc_uint32	_alignment_1_;				//	32
	xc_uint64 	ParentDirLotIndex;			//	40
	xc_uint64	FileSize;					//	48	
	xc_uint64	AllocationSize;				//	56
	xc_uint64	LotIndex;					//	64
	xc_uint64	AddressMapIndex;			//	72
	xc_uint64 	Create_time;				// 	80	
	xc_uint64 	Change_time;				//	88	
	xc_uint64	Modified_time;				// 	96
	xc_uint64 	Access_time;				// 	104
	xc_uint8	OwnHostId[16];				//	120
	xc_uint8	Reserved1[392];				// 	512
	xc_uint8 	Name[2048];					//	2560	(512*5)
	xc_uint8	Reserved2[1512];			//	4072
	xc_uint64	SequenceNum;				//	4080
	xc_uint8	Digest[16];					//	4096	(512*8)
} XIDISK_FILE_INFO, *PXIDISK_FILE_INFO;


XC_CASSERT_SIZEOF(XIDISK_FILE_INFO,4096);

#define XIDISK_FILE_INFO_SIZE	(SECTORALIGNSIZE_4096(sizeof(XIDISK_FILE_INFO)))
/*
/
/
/	Xifs file system structure
/
/
*/
typedef  struct _XIDISK_COMMON_LOT_HEADER {
	XIDISK_LOCK					Lock;			//	512
	XIDISK_LOT_INFO				LotInfo;		// 	1024
	xc_uint8					Reserved[3048];	//	4072
	xc_uint64					SequenceNum;	//	4080
	xc_uint8					Digest[16];		//	4096
}  XIDISK_COMMON_LOT_HEADER, *PXIDISK_COMMON_LOT_HEADER;

XC_CASSERT_SIZEOF(XIDISK_COMMON_LOT_HEADER,4096);

#define XIDISK_COMMON_LOT_HEADER_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_COMMON_LOT_HEADER)))


typedef  struct _XIDISK_DIR_HEADER_LOT {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096*2
	XIDISK_DIR_INFO				DirInfo;		//	4096*3
	XIDISK_DIR_INFO				_Du_DirInfo_;	//	4096*4
} XIDISK_DIR_HEADER_LOT, *PXIDISK_DIR_HEADER_LOT;

XC_CASSERT_SIZEOF(XIDISK_DIR_HEADER_LOT, 16384);

#define XIDISK_DIR_HEADER_LOT_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_DIR_HEADER_LOT)))

typedef  struct _XIDISK_DIR_HEADER {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096*2
	XIDISK_DIR_INFO 			DirInfo;		//	4096*3
	XIDISK_DIR_INFO				_Du_DirInfo_;	//	4096*4
	XIDISK_ADDR_MAP				AddrInfo;		//	16*8*1024 + 4096*4
} XIDISK_DIR_HEADER, *PXIDISK_DIR_HEADER;

XC_CASSERT_SIZEOF(XIDISK_DIR_HEADER, 147456);

#define XIDISK_DIR_HEADER_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_DIR_HEADER)))

typedef struct   _XIDISK_FILE_HEADER_LOT {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096*2
	XIDISK_FILE_INFO			FileInfo;		//	4096*3
	XIDISK_FILE_INFO			_Du_FileInfo_;	//	4096*4
} XIDISK_FILE_HEADER_LOT, *PXIDISK_FILE_HEADER_LOT;

XC_CASSERT_SIZEOF(XIDISK_FILE_HEADER_LOT, 16384);

#define XIDISK_FILE_HEADER_LOT_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_FILE_HEADER_LOT)))

typedef  struct _XIDISK_FILE_HEADER {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096*2
	XIDISK_FILE_INFO			FileInfo;		//	4096*3
	XIDISK_FILE_INFO			_Du_FileInfo_;	//	4096*4
	XIDISK_ADDR_MAP				AddrInfo;		//	16*8*1024 + 4096*4
} XIDISK_FILE_HEADER, *PXIDISK_FILE_HEADER;

XC_CASSERT_SIZEOF(XIDISK_FILE_HEADER, 147456);

#define XIDISK_FILE_HEADER_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_FILE_HEADER)))

typedef  struct _XIDISK_DATA_LOT{
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096*2
} XIDISK_DATA_LOT, *PXIDISK_DATA_LOT;

XC_CASSERT_SIZEOF(XIDISK_DATA_LOT, 8192);

#define XIDISK_DATA_LOT_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_DATA_LOT)))

typedef  struct _XIDISK_VOLUME_LOT {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096 * 2
	XIDISK_VOLUME_INFO			VolInfo;		//	4096 * 3
	XIDISK_VOLUME_INFO			_Du_VolInfo_;	//	4096 * 4		
} XIDISK_VOLUME_LOT, *PXIDISK_VOLUME_LOT;

XC_CASSERT_SIZEOF(XIDISK_VOLUME_LOT, 16384);

#define XIDISK_VOLUME_LOT_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_VOLUME_LOT)))

typedef  struct _XIDISK_MAP_LOT {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096 * 2
	XIDISK_LOT_MAP_INFO			Map;			//	4096 * 3
	XIDISK_LOT_MAP_INFO			_Du_Map_;		//	4096 * 4
} XIDISK_MAP_LOT, *PXIDISK_MAP_LOT;
#define XIDISK_MAP_LOT_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_MAP_LOT)))

XC_CASSERT_SIZEOF(XIDISK_MAP_LOT, 16384);

typedef  struct _XIDISK_HOST_REG_LOT {
	XIDISK_COMMON_LOT_HEADER	LotHeader;		//	4096
	XIDISK_COMMON_LOT_HEADER	_Du_LotHeader_;	//	4096 * 2
	XIDISK_HOST_INFO			HostInfo;		//	4096 * 3
	XIDISK_HOST_INFO			_Du_HostInfo_;	//	4096 * 4
} XIDISK_HOST_REG_LOT, *PXIDISK_HOST_REG_LOT;

XC_CASSERT_SIZEOF(XIDISK_HOST_REG_LOT, 16384);

#define XIDISK_HOST_REG_LOT_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_HOST_REG_LOT)))



/*	
	du meta header size definition.
	used for Raw disk read operation.
*/

typedef  struct _XIDISK_DUP_COMMON_LOT_HEADER {
	XIDISK_COMMON_LOT_HEADER	LotHeaderEven;			//	4096
	XIDISK_COMMON_LOT_HEADER	LotHeaderOdd;			//	4096 *2
}  XIDISK_DUP_COMMON_LOT_HEADER, *PXIDISK_DUP_COMMON_LOT_HEADER;

typedef   struct _XIDISK_DUP_HASH_VALUE_TABLE{
	XIDISK_HASH_VALUE_TABLE HashTableEven;				//4096
	XIDISK_HASH_VALUE_TABLE HashTableOdd;				//4096 *2
} XIDISK_DUP_HASH_VALUE_TABLE, *PXIDISK_DUP_HASH_VALUE_TABLE;

typedef  struct _XIDISK_DUP_DIR_INFO {
	XIDISK_DIR_INFO DirInfoEven;						//4096
	XIDISK_DIR_INFO DirInfoOdd;							//4096 *2
}  XIDISK_DUP_DIR_INFO, *PXIDISK_DUP_DIR_INFO;

typedef  struct _XIDISK_DUP_VOLUME_INFO{
	XIDISK_VOLUME_INFO	VolInfoEven;					//4096
	XIDISK_VOLUME_INFO	VolInfoOdd;						//4096 *2
}  XIDISK_DUP_VOLUME_INFO, *PXIDISK_DUP_VOLUME_INFO;

typedef  struct _XIDISK_DUP_CHILD_INFORMATION{
	XIDISK_CHILD_INFORMATION	ChildInfoEven;			//4096
	XIDISK_CHILD_INFORMATION	ChildInfoOdd;			//4096 *2
} XIDISK_DUP_CHILD_INFORMATION, *PXIDISK_DUP_CHILD_INFORMATION;

typedef  struct _XIDISK_DUP_LOT_MAP_INFO{
	XIDISK_LOT_MAP_INFO LotMapInfoEven;					//4096
	XIDISK_LOT_MAP_INFO LotMapInfoOdd;					//4096 *2
}  XIDISK_DUP_LOT_MAP_INFO, *PXIDISK_DUP_LOT_MAP_INFO;

typedef  struct _XIDISK_DUP_HOST_INFO{
	XIDISK_HOST_INFO	HostInfoEven;					//4096
	XIDISK_HOST_INFO	HostInfoOdd;					//4096 *2
} XIDISK_DUP_HOST_INFO, *PXIDISK_DUP_HOST_INFO;

typedef  struct _XIDISK_DUP_HOST_RECORD {
	XIDISK_HOST_RECORD	HostRecordEven;					//4096
	XIDISK_HOST_RECORD	HostRecordOdd;					//4096 *2
}  XIDISK_DUP_HOST_RECORD, *PXIDISK_DUP_HOST_RECORD;


#define XIDISK_DUP_COMMON_LOT_HEADER_SIZE 	(SECTORALIGNSIZE_4096(sizeof(XIDISK_COMMON_LOT_HEADER) *2))
#define XIDISK_DUP_VOLUME_INFO_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_VOLUME_INFO) *2 )) 
#define XIDISK_DUP_LOT_MAP_INFO_SIZE		(SECTORALIGNSIZE_4096(sizeof(XIDISK_LOT_MAP_INFO) *2)) 
#define XIDISK_DUP_HOST_RECORD_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_HOST_RECORD) *2)) 
#define XIDISK_DUP_HOST_INFO_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_HOST_INFO) *2)) 
#define XIDISK_DUP_DIR_INFO_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_DIR_INFO) *2))
#define XIDISK_DUP_HASH_VALUE_TABLE_SIZE	(SECTORALIGNSIZE_4096(sizeof(XIDISK_HASH_VALUE_TABLE) *2))
#define XIDISK_DUP_FILE_INFO_SIZE			(SECTORALIGNSIZE_4096(sizeof(XIDISK_FILE_INFO) *2))

#include "xcsystem/poppack.h"

#endif //#ifndef __XIXCORE_LAYOUTS__H__
