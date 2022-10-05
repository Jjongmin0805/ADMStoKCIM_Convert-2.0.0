#pragma once
#include <atldbcli.h>
#include <atldbsch.h>
#include "Node.h"
#include "Branch.h"
#include "ADMS.h"
#include "stdafx.h"
#include "afxdb.h"

// CADMStoKCIMDlg 대화 상자
typedef CArray<HEAD_STA, HEAD_STA&> CHEAD_STA;
typedef CArray<CENTER_STA, CENTER_STA&> CCENTER_STA;
typedef CArray<MEMBER_STA, MEMBER_STA&> CMEMBER_STA;
typedef CArray<SS_STA, SS_STA&> CSS_STA;
typedef CArray<MTR_STA, MTR_STA&> CMTR_STA;
typedef CArray<DL_STA, DL_STA&> CDL_STA;
typedef CArray<ND_STA, ND_STA&> CND_STA;
typedef CArray<CBSW_STA, CBSW_STA&> CCBSW_STA;
typedef CArray<GEN_STA, GEN_STA&> CGEN_STA;
typedef CArray<GENUNIT_STA, GENUNIT_STA&> CGENUNIT_STA;
typedef CArray<LD_STA, LD_STA&> CLD_STA;
typedef CArray<HVCUS_STA, HVCUS_STA&> CHVCUS_STA;
typedef CArray<LNSEC_STA, LNSEC_STA&> CLNSEC_STA;
typedef CArray<TR_STA, TR_STA&> CTR_STA;

class AFX_EXT_CLASS CADMStoKCIMDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CADMStoKCIMDlg)

public:
	CADMStoKCIMDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CADMStoKCIMDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADMSTOKCIM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


protected:
	HICON				m_hIcon;

	CDatabase			m_ADMSDB;
	CDatabase			m_ADMSCDDB;
	CDatabase			m_ADMSAPPLICATIONDB;
	CProgressCtrl		m_progress;

public:
	int					m_nSuccess;
	//DB이름 
	CString				m_szTime;
	CString				m_szDataName_Data;
	CString				m_szADMSDB_Office;
	CString				m_szADMSDB_Code;
	CString				m_szADMSDB_Application;
	CString				m_szCSV_Route;
	CString				m_szADMS_Code;

public:
	virtual BOOL		OnInitDialog();
	CString				MyPath();
	void				OnTimer(UINT_PTR nIDEvent);

public:
	void				ADMStoKCIM_Offic_Config( CString szDate );   //연결!
	void				ADMStoKCIM_Code_Config(CString szDate);	 //연결!
	void				ADMStoKCIM_Application_Config(CString szDate);	 //연결!
	void				ADMStoKCIM_CSV_Route( CString szDate );		 //연결!	
	void				MakeDirectory();
	void				RemoveAllData();
	void				ADMStoKCIM_Config();
	void				DeleteAllFiles(CString dirName); //폴더 기존 데티어 삭제
	void				ADMStoKCIM_RemoveAll();

	void				NEW_SUBSTATION_INSERT(int nSS_ID, CString stSS_NM);
	void				NEW_MTR_INSERT(CString strMRID, CString stNAME, int nMTR_ID);
	void				NEW_DS_OCB();
	int					NEW_CBSW_TYPE(int nCeqType, int nMULTICIR_NUMBER);
	int					NEW_CBSW_MAKE_NO(int nCeqType, int nMULTICIR_NUMBER);
	int					NEW_CBSW_MULTICIR_NUMBER_TYPE_CHANG(int nCeqType, int nMULTICIR_NUMBER);


	void				NEW_ADMStoKCIM_Read();
	void				NEW_ADMStoKCIM_Insert();			 //KASIM 생성

public: //KASIM 배열
	CHEAD_STA			m_arrHEAD_STA;
	CCENTER_STA			m_arrCENTER_STA;
	CMEMBER_STA			m_arrMEMBER_STA;
	CSS_STA				m_arrSS_STA;
	CMTR_STA			m_arrMTR_STA;
	CDL_STA				m_arrDL_STA;
	CND_STA				m_arrND_STA;
	CCBSW_STA			m_arrCBSW_STA;
	CGEN_STA			m_arrGEN_STA;
	CGENUNIT_STA		m_arrGENUNIT_STA;
	CLD_STA				m_arrLD_STA;
	CHVCUS_STA			m_arrHVCUS_STA;
	CLNSEC_STA			m_arrLNSEC_STA;
	CTR_STA				m_arrTR_STA;

public: //MAP
	CMap<CString, LPCTSTR, CString, LPCTSTR>m_map_NEW_DS1_CBSW_CEQ_ND; //SUBS에 연결된 LNSEC TND 노드 찾기
	CMap<CString, LPCTSTR, CString, LPCTSTR>m_map_NEW_DS2_CBSW_CEQ_ND; //SUBS에 연결된 LNSEC TND 노드 찾기
	CMap<CString, LPCTSTR, CString, LPCTSTR>m_map_NEW_CBMRID_MTRMRID; //CBSW 오픈온 정보 1 = 0 바꿔야함	   
	CMap<CString, LPCTSTR, int, int>m_map_NEW_CBSW_bStatus; //CBSW 오픈온 정보 1 = 0 바꿔야함	   
	CMap<CString, LPCTSTR, int, int>m_map_NEW_GEN_NDisnull; //GEN정보가 이상함 연결정보가 없으면!
protected:
	int				GetMasterCD(int nCeqTp, int nCircuit);
	int				GetMasterCode(int nMst, int nCeqTp);

	CProgressCtrl			m_ctrProgressADMStoKCIM;
	CListBox				m_ListCtrl_Data;

public:
	//////////////

	void					PumpMessages();
	void					IDC_LIST_DATA_HISTORY(CString  strData_Name);
	CString					LIST_Current_Time();
	CString					LIST_Current_Time_Kasim();

public:
	int 			m_nMstCD[500][3];
	virtual			BOOL DestroyWindow();
	int				GetSuccess();
};
