#import "macsupport.h"

/* Portions of CPS.h */
typedef struct CPSProcessSerNum
{
	UInt32          lo;
	UInt32          hi;
} CPSProcessSerNum;

extern OSErr    CPSGetCurrentProcess( CPSProcessSerNum *psn);
extern OSErr    CPSEnableForegroundOperation( CPSProcessSerNum *psn, UInt32 
											 _arg2, UInt32 _arg3, UInt32 _arg4, UInt32 _arg5);
extern OSErr    CPSSetFrontProcess( CPSProcessSerNum *psn);

void CustomSDLMain()
{
	NSAutoreleasePool  *pool = [[NSAutoreleasePool alloc] init];
	[ NSApplication sharedApplication ];
	[ NSApp setMainMenu:[[NSMenu alloc] init] ];
	
	{
		CPSProcessSerNum PSN;
		/* Tell the dock about us */
		if (!CPSGetCurrentProcess(&PSN))
			if (!CPSEnableForegroundOperation(&PSN,0x03,0x3C,0x2C,0x1103))
				if (!CPSSetFrontProcess(&PSN))
					[NSApplication sharedApplication];
	}
}
