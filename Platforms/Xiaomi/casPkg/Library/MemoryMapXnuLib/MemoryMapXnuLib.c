// SPDX-License-Identifier: BSD-2-Clause-Patent
// The verified cas map plus one conventional high-RAM region for the XNU owned window.
#include <CasPlatform.h>
#define GetMemoryMap CasDefaultGetMemoryMap
#include "../MemoryMapLib/MemoryMapLib.c"
#undef GetMemoryMap
STATIC BOOLEAN mApplied=FALSE;
STATIC EFI_MEMORY_REGION_DESCRIPTOR mXnuMap[ARRAY_SIZE(mRegions)+1];
STATIC UINT8 mXnuCount;
VOID GetMemoryMap(EFI_MEMORY_REGION_DESCRIPTOR **Map,UINT8 *Count) {
  UINTN I;EFI_MEMORY_REGION_DESCRIPTOR *Default;UINT8 Number;
  CONST EFI_MEMORY_REGION_DESCRIPTOR High={"XNU High RAM",CAS_XNU_HIGH_BASE,CAS_XNU_HIGH_SIZE,
    AddMem,SYS_MEM,SYS_MEM_CAP,Conv,ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK};
  if(mApplied) {*Map=mXnuMap;*Count=mXnuCount;return;}
  CasDefaultGetMemoryMap(&Default,&Number);*Map=Default;*Count=Number;
  if(Number!=ARRAY_SIZE(mRegions) || ARRAY_SIZE(mXnuMap)>=128)return;
  for(I=0;I<Number;++I) {
    EFI_MEMORY_REGION_DESCRIPTOR *R=&Default[I];
    // Refuse the extension, keeping the verified map, if anything overlaps it.
    if(!R->Length || R->Address>(~0ULL)-R->Length ||
       (R->Address<CAS_XNU_HIGH_BASE+CAS_XNU_HIGH_SIZE && CAS_XNU_HIGH_BASE<R->Address+R->Length))return;
    mXnuMap[I]=Default[I];
  }
  mXnuMap[Number]=High;mXnuCount=Number+1;mApplied=TRUE;
  *Map=mXnuMap;*Count=mXnuCount;
}
