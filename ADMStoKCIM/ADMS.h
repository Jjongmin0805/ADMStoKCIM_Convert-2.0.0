#pragma once

#ifndef __KASIM_DEFINE__
#define __KASIM_DEFINE__

typedef struct _HEAD_STA
{
	wchar_t							HEAD_ID[64];
	wchar_t							HEAD_NM[64];

} HEAD_STA;
#define								HEAD_STA_SIZE					sizeof( HEAD_STA )

typedef struct _CENTER_STA
{
	wchar_t							CENTER_ID[64];
	wchar_t							CENTER_NM[64];
	wchar_t							CENTER_II_HDOF[64];

} CENTER_STA;
#define								CENTER_STA_SIZE					sizeof( CENTER_STA )

typedef struct _MEMBER_STA
{
	wchar_t							MEMBER_ID[64];
	wchar_t							MEMBER_NM[64];
	wchar_t							MEMBER_II_CENTER[64];

} MEMBER_STA;
#define								MEMBER_STA_SIZE					sizeof( MEMBER_STA )

typedef struct _SS_STA
{
	wchar_t							SS_ID[64];
	wchar_t							SS_NM[64];
	wchar_t							SS_CODE[64];
	wchar_t							SS_II_HEAD[64];

} SS_STA;
#define								SS_STA_SIZE					sizeof( SS_STA )

typedef struct _MTR_STA
{
	wchar_t							MTR_ID[64];
	wchar_t							MTR_NM[64];
	int								MTR_BANK;
	wchar_t							MTR_II_SS[64];

} MTR_STA;
#define								MTR_STA_SIZE					sizeof( MTR_STA )

typedef struct _DL_STA
{
	wchar_t							DL_ID[64];
	wchar_t							DL_NM[64];
	wchar_t							DL_II_MEMBER[64];
	wchar_t							DL_II_MTR[64];
	wchar_t							DL_II_CB[64];

} DL_STA;
#define								DL_STA_SIZE					sizeof( DL_STA )

typedef struct _ND_STA
{
	wchar_t							ND_ID[64];
	wchar_t							ND_NM[64];

} ND_STA;
#define								ND_STA_SIZE					sizeof( ND_STA )

typedef struct _CBSW_STA
{
	wchar_t							CBSW_ID[64];
	wchar_t							CBSW_NM[64];
	int								CBSW_TYPE;
	int								CBSW_RTUTYPE;
	int								CBSW_RTUCODE;
	int								CBSW_COMTYPE;
	int								CBSW_NORSTAT;
	wchar_t							CBSW_II_FND[64];
	wchar_t							CBSW_II_TND[64];
	wchar_t							CBSW_MULTISW_ID[64];
	int								CBSW_MULTICIR_NUMBER;

	int								CBSW_MEAUSE;
	int								CBSW_MANF;
	int								CBSW_SRCDIRECTION;
	int								CBSW_PFSIGN;
	int								CBSW_DIRREF;
	int								CBSW_BASE_STATE;

} CBSW_STA;
#define								CBSW_STA_SIZE					sizeof( CBSW_STA )

// typedef struct _CBSW_DYN_UIN
// {
// 	int								CBSW_MEAUSE;
// 	int								CBSW_MANF;
// 	int								CBSW_SRCDIRECTION;
// 	int								CBSW_PFSIGN;
// 	int								CBSW_DIRREF;
// 	int								CBSW_BASE_STATE;
// 
// } CBSW_DYN_UIN;
// #define								CBSW_DYN_UIN_SIZE					sizeof( CBSW_DYN_UIN )

// typedef struct _CBSW_DYN_MEA
// {
// 	int								CBSW_ODSTAT;
// 	int								CBSW_NWSTAT;
// 	double							CBSW_PAMEAKV;
// 	double							CBSW_PAMEAKV_OLD;
// 	double							CBSW_PBMEAKV;
// 	double							CBSW_PBMEAKV_OLD;
// 	double							CBSW_PCMEAKV;
// 	double							CBSW_PCMEAKV_OLD;
// 	double							CBSW_PAMEAAMP;
// 	double							CBSW_PAMEAAMP_OLD;
// 	double							CBSW_PBMEAAMP;
// 	double							CBSW_PBMEAAMP_OLD;
// 	double							CBSW_PCMEAAMP;
// 	double							CBSW_PCMEAAMP_OLD;
// 	double							CBSW_PAMEAADIFF;
// 	double							CBSW_PAMEAADIFF_OLD;
// 	double							CBSW_PBMEAADIFF;
// 	double							CBSW_PBMEAADIFF_OLD;
// 	double							CBSW_PCMEAADIFF;
// 	double							CBSW_PCMEAADIFF_OLD;
// 	int								CBSW_COMSTATSER;
// 	int								CBSW_CONTSTAT;
// 	int								CBSW_DIRSET;
// 
// } CBSW_DYN_MEA;
// #define								CBSW_DYN_MEA_SIZE					sizeof( CBSW_DYN_MEA )

typedef struct _GEN_STA
{
	wchar_t							GEN_ID[64];
	wchar_t							GEN_NM[64];
	int								GEN_TYPE;
	wchar_t							GEN_II_ND[64];

	int								GEN_ISMEASURE;
	wchar_t							GEN_MEA_SW[64];

} GEN_STA;
#define								GEN_STA_SIZE					sizeof( GEN_STA )


typedef struct _GENUNIT_STA
{
	wchar_t							GENUNIT_ID[64];
	wchar_t							GENUNIT_NM[64];
	wchar_t							GENUNIT_CONNECT_CEQID[64];
	double							GENUNIT_CAP_KW;
	int								GENUNIT_TYPE;
	int								GENUNIT_CONTRACT_TYPE;
	int								GENUNIT_CONTRACT_STATUS;

} GENUNIT_STA;
#define								GENUNIT_STA_SIZE					sizeof( GENUNIT_STA )

typedef struct _LD_STA
{
	wchar_t							LD_ID[64];
	wchar_t							LD_NM[64];
	wchar_t							LD_II_ND[64];

	double							LD_DIV;

} LD_STA;
#define								LD_STA_SIZE					sizeof( LD_STA )

// typedef struct _LD_DYN_UIN
// {
// 	double							LD_DIV;
// 
// } LD_DYN_UIN;
// #define								LD_DYN_UIN_SIZE					sizeof( LD_DYN_UIN )

typedef struct _HVCUS_STA
{
	wchar_t							HVCUS_ID[64];
	wchar_t							HVCUS_NM[64];
	wchar_t							HVCUS_II_ND[64];
	double							HVCUS_CON_KVA;

} HVCUS_STA;
#define								HVCUS_STA_SIZE					sizeof( HVCUS_STA )

typedef struct _LNSEC_STA
{
	wchar_t							LNSEC_ID[64];
	wchar_t							LNSEC_NM[64];
	wchar_t							LNSEC_II_FND[64];
	wchar_t							LNSEC_II_TND[64];
	double							LNSEC_POSR;
	double							LNSEC_POSX;
	double							LNSEC_ZERR;
	double							LNSEC_ZERX;
	double							LNSEC_THRLM;
	double							LNSEC_LENGTH;

	int								LNSEC_SW_FOR_CONST;

} LNSEC_STA;
#define								LNSEC_STA_SIZE					sizeof( LNSEC_STA )

// typedef struct _LNSEC_DYN_UIN
// {
// 	int								LNSEC_SW_FOR_CONST;
// 
// } LNSEC_DYN_UIN;
// #define								LNSEC_DYN_UIN_SIZE					sizeof( LNSEC_DYN_UIN )

typedef struct _TR_STA
{
	wchar_t							TR_ID[64];
	wchar_t							TR_NM[64];
	wchar_t							TR_II_FND[64];
	wchar_t							TR_II_TND[64];
	int								TR_TYPE;
	double							TR_FNORKV;
	double							TR_TNORKV;
	double							TR_POSX;
	double							TR_ZERX;
	wchar_t							TR_II_MTR[64];

} TR_STA;
#define								TR_STA_SIZE					sizeof( TR_STA )

#endif