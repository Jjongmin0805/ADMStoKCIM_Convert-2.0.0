// CADMStoKCIMDlg.cpp: 구현 파일
//
#define MY_TIMER		100
#define LC_ALL			0

#include "stdafx.h"
#include "CADMStoKCIMDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "XMLMgr.h"
#include <math.h>
#include "TcrMathCoord.h"
//#include "../Include/DBContainer/DB_EXTRACT.h"


// CADMStoKCIMDlg 대화 상자

IMPLEMENT_DYNAMIC(CADMStoKCIMDlg, CDialogEx)

CADMStoKCIMDlg::CADMStoKCIMDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADMSTOKCIM, pParent)
{
	m_szADMSDB_Office.Format(_T(""));
	m_szADMSDB_Code.Format(_T(""));
	m_szTime.Format(_T(""));
	m_szDataName_Data.Format(_T(""));
	m_szCSV_Route.Format(_T(""));
	m_szADMS_Code.Format(_T(""));
	m_nSuccess = 0;     // 
	for (int i = 0; i < 500; i++)
	{
		m_nMstCD[i][0] = 0;
		m_nMstCD[i][1] = 0;
		m_nMstCD[i][2] = 0; 
	}
	//ttsesdsdsdsdsd
}

CADMStoKCIMDlg::~CADMStoKCIMDlg()
{
	ADMStoKCIM_RemoveAll();
}

void CADMStoKCIMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_ADMStoKCIM, m_ctrProgressADMStoKCIM);
	DDX_Control(pDX, IDC_LIST_ADMStoKCIM, m_ListCtrl_Data);
}

BEGIN_MESSAGE_MAP(CADMStoKCIMDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CADMStoKCIMDlg::OnInitDialog()
{
	//20221005 업데이트
	CDialogEx::OnInitDialog();

	int nProgress;
	nProgress = 5;
	m_ctrProgressADMStoKCIM.SetRange(0, nProgress);	 //범위 0~200
	m_ctrProgressADMStoKCIM.SetPos(0);				 //0으로 초기화
	m_ctrProgressADMStoKCIM.SetStep(1);				 //1씩 증가   

	SetTimer(MY_TIMER, 1000, NULL);
	_wsetlocale(LC_ALL, _T("kor"));	
	return TRUE;  // return TRUE unless you set the focus to a control				  
}

BOOL CADMStoKCIMDlg::DestroyWindow()
{
	m_ADMSDB.Close();
	m_ADMSCDDB.Close();
	m_ADMSAPPLICATIONDB.Close();
	return CDialogEx::DestroyWindow();
}

int CADMStoKCIMDlg::GetSuccess()
{
	return m_nSuccess;
}

void CADMStoKCIMDlg::RemoveAllData()	
{
	// 배열 삭제
	ADMStoKCIM_RemoveAll();
}
// 필수 함수 입력부분
CString CADMStoKCIMDlg::MyPath()
{
	CString slmpath;
	WCHAR szDirve[256], szDir[256];
	WCHAR programpath[2048];
	GetModuleFileName(0, programpath, 1024);
	_wsplitpath(programpath, szDirve, szDir, NULL, NULL);
	slmpath.Format(_T("%s%s"), szDirve, szDir);

	return slmpath.Left(slmpath.GetLength() - 1);
}

//프로그램 시작 
void CADMStoKCIMDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case MY_TIMER:
		{
			//2.0
			KillTimer(MY_TIMER);
			ADMStoKCIM_Config();	 //ADMS DB연결
			//강제로 종료?
			if (m_nSuccess == 1)
			{
				CDialogEx::OnOK();
				KillTimer(MY_TIMER);
				break;
			}
			MakeDirectory();	   //폴도 생성 부분 !!! 파일 삭제 부분 

			m_szTime = LIST_Current_Time();
			m_szDataName_Data.Format(_T("%s[1/6]========== ADMStoKASIM_2.0.1 Start =========="), m_szTime);
			IDC_LIST_DATA_HISTORY(m_szDataName_Data);
			m_ctrProgressADMStoKCIM.StepIt();  //1
					
			m_szTime = LIST_Current_Time();
			m_szDataName_Data.Format(_T("%s[2/6]========== ADMStoKASIM_2.0.1 DB Read =========="), m_szTime);
			IDC_LIST_DATA_HISTORY(m_szDataName_Data);
			m_ctrProgressADMStoKCIM.StepIt(); //2
			NEW_ADMStoKCIM_Read();		 //ADMS 읽어오기 ----------------------------------------------------------------

			m_szTime = LIST_Current_Time();
			m_szDataName_Data.Format(_T("%s[3/6]========== ADMStoKASIM_2.0.1 Convert =========="), m_szTime);
			IDC_LIST_DATA_HISTORY(m_szDataName_Data);
			m_ctrProgressADMStoKCIM.StepIt(); //3
			//
			NEW_DS_OCB();

			m_szTime = LIST_Current_Time();
			m_szDataName_Data.Format(_T("%s[5/6]========== ADMStoKASIM_2.0.1 CSV Insert =========="), m_szTime);
			IDC_LIST_DATA_HISTORY(m_szDataName_Data);
			m_ctrProgressADMStoKCIM.StepIt(); //5
			//
			NEW_ADMStoKCIM_Insert();     //KCIM DBINSERT----------------------------------------------------------------

			m_szTime = LIST_Current_Time();
			m_szDataName_Data.Format(_T("%s[6/6]========== ADMStoKASIM_2.0.1 CSV END =========="), m_szTime);
			IDC_LIST_DATA_HISTORY(m_szDataName_Data);
			m_ctrProgressADMStoKCIM.StepIt(); //5
		   
			CDialogEx::OnOK();
		}
		KillTimer(MY_TIMER);
		break;
	}
}

CString CADMStoKCIMDlg::LIST_Current_Time()
{
	CString strTime;
	CTime cTime = CTime::GetCurrentTime();
	strTime.Format(_T("[%04d %02d.%02d %02d:%02d:%02d ]"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond()); // 현재 초 반환

	return strTime;
}

CString CADMStoKCIMDlg::LIST_Current_Time_Kasim()
{
	CString strTime;
	CTime cTime = CTime::GetCurrentTime();
	strTime.Format(_T("%04d%02d%02d"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay()); // 현재 초 반환

	return strTime;
}

//DB연결 하는 부분 - 받아오는 부분 1
void CADMStoKCIMDlg::ADMStoKCIM_Offic_Config(CString szDate) 
{
	m_szADMSDB_Office.Format(_T("%s"), szDate);
}
//DB연결 하는 부분 - 받아오는 부 2
void CADMStoKCIMDlg::ADMStoKCIM_Code_Config(CString szDate)
{
	m_szADMSDB_Code.Format(_T("%s"), szDate);
}
//DB연결 하는 부분 - 받아오는 부분 3
void CADMStoKCIMDlg::ADMStoKCIM_Application_Config(CString szDate)
{
	m_szADMSDB_Application.Format(_T("%s"), szDate);
}

//DB연결 하는 부분 - 받아오는 부분 4
void CADMStoKCIMDlg::ADMStoKCIM_CSV_Route(CString szDate)
{
	m_szCSV_Route.Format(_T("%s"), szDate);
}

void CADMStoKCIMDlg::MakeDirectory()
{
	//파일 삭제 부분 
	CString szRoute, stream;
	szRoute.Format(_T("%sDUAL"), m_szCSV_Route);
	stream = (MyPath() + szRoute);
	DeleteAllFiles(stream);
	szRoute.Format(_T("%sST"), m_szCSV_Route);
	stream = (MyPath() + szRoute);
	DeleteAllFiles(stream);
}

void CADMStoKCIMDlg::ADMStoKCIM_RemoveAll()
{
	m_arrHEAD_STA.RemoveAll();
	m_arrCENTER_STA.RemoveAll();
	m_arrMEMBER_STA.RemoveAll();
	m_arrSS_STA.RemoveAll();
	m_arrMTR_STA.RemoveAll();
	m_arrDL_STA.RemoveAll();
	m_arrND_STA.RemoveAll();
	m_arrCBSW_STA.RemoveAll();
	m_arrGEN_STA.RemoveAll();
	m_arrGENUNIT_STA.RemoveAll();
	m_arrLD_STA.RemoveAll();
	m_arrHVCUS_STA.RemoveAll();
	m_arrLNSEC_STA.RemoveAll();
	m_arrTR_STA.RemoveAll();

	m_ADMSDB.Close();
	m_ADMSCDDB.Close();
	m_ADMSAPPLICATIONDB.Close();
}

void CADMStoKCIMDlg::ADMStoKCIM_Config()
{
	if (m_ADMSDB.IsOpen())
	{
		return;
	}
	if (m_ADMSCDDB.IsOpen())
	{
		return;
	}		
	if (m_ADMSAPPLICATIONDB.IsOpen())
	{
		return;
	}
	//확인사항!
	for (int i = 0 ; i < 30; i++)
	{
		try
		{
			if (m_ADMSDB.OpenEx(m_szADMSDB_Office, CDatabase::openReadOnly | CDatabase::noOdbcDialog))
			{
				i = 100;
			}
		}
		catch (CDBException * e)
		{
			_tprintf(_T("%s"), e->m_strError.GetBuffer());
			Sleep(10000);
		}
	}
	if (!(m_ADMSDB.IsOpen()))
	{
		m_nSuccess = 1;
		return;
	}
	for (int i = 0; i < 30; i++)
	{
		try
		{
			if (m_ADMSCDDB.OpenEx(m_szADMSDB_Code, CDatabase::openReadOnly | CDatabase::noOdbcDialog))
			{
				i = 100;
			}
		}
		catch (CDBException * e)
		{
			_tprintf(_T("%s"), e->m_strError.GetBuffer());
			Sleep(10000);
		}
	}
	if (!(m_ADMSCDDB.IsOpen()))
	{
		m_nSuccess = 1;
		return;
	}	
	for (int i = 0; i < 30; i++)
	{
		try
		{
			if (m_ADMSAPPLICATIONDB.OpenEx(m_szADMSDB_Application, CDatabase::openReadOnly | CDatabase::noOdbcDialog))
			{
				i = 100;
			}
		}
		catch (CDBException * e)
		{
			_tprintf(_T("%s"), e->m_strError.GetBuffer());
			Sleep(10000);
		}
	}
	if (!(m_ADMSAPPLICATIONDB.IsOpen()))
	{
		m_nSuccess = 1;
		return;
	}
}

void CADMStoKCIMDlg::NEW_ADMStoKCIM_Read()
{
	CRecordset rs(&m_ADMSDB);
	CRecordset rs_code(&m_ADMSCDDB);
	CRecordset rs_Application(&m_ADMSAPPLICATIONDB);
	//공용
	CString strData;
	CString stNAME;
	CString szFND_CN;
	CString szTND_CN;
	//HEAD
	CString szHEAD_ID;
	CString	szHDOF_NM;
	int HEAD_ID = 0;
	//CENTER
	CString szCENTER_MRID;
	//MEMBER
	CString szMEMBER_MRID;
	//SS
	CString szSS_MRID;
	CString szSS_CODE;
	//MTR
	CString szMTR_MRID;
	int nMTR_BANK_NO = 0;
	//DL
	CString szDL_MRID;
	CString szDL_CB_MRID;
	//ND
	CString szND_MRID;
	//CBSW
	CString szCBSW_MRID;
	CString szMUCBSW_MRID;
	int nMU_Munber = 0;
	int nCEQ_TYPE = 0;
	int nCbsw_RTUCODE = 0;
	int nCbsw_ComType = 0;
	int nbStatus = 0;
	int nCBSW_TYPE = 0;
	int nContinue = 0;
	//GEN 
	CString szGEN_MRID;
	//GENUNIT
	CString szGENUNIT_MRID;
	float fGENUNIT_CAPACITY;
	int nGENUNIT_TYPE;
	int nGENUNIT_CONTRACT_TYPE;
	int nGENUNIT_CONTRACT_STATUS;
	//HVCUS
	CString szHVCUS_MRID;
	int nHVCUS_CON_KVA;
	//LNSEC
	CString szLNSEC_MRID;
	int nNAME_TYPE;
	int nLNSEC_TYPE;
	float fLNSEC_LENGTH;
	//TR
	CString szTR_MRID;

	CString szADMS_Code;
	int nn1 = 0;
	try
	{
		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[1/15]========== 공용 MAP =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);
		//공용 필요한거 MAP
		////CBSW ON/OFF를 찾기위한 마스터 코드
		CString stMaster_codeID, stMasterCEQ_TYPE, stMaster_Circuit;
		int nMaster_codeID, nMasterCEQ_TYPE, nMaster_Circuit;
		int nMaster_Index = 0;
		szADMS_Code.Format(L"select master_code_id, ceq_type_fk, circuit from master_code where   point_type_code_fk = 3 and code_fk = 1");
		if (rs_code.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");

		while (!rs_code.IsEOF())
		{
			rs_code.GetFieldValue((short)0, stMaster_codeID);
			nMaster_codeID = _wtoi(stMaster_codeID);
			rs_code.GetFieldValue((short)1, stMasterCEQ_TYPE);
			nMasterCEQ_TYPE = _wtoi(stMasterCEQ_TYPE);
			rs_code.GetFieldValue((short)2, stMaster_Circuit);
			nMaster_Circuit = _wtoi(stMaster_Circuit);

			m_nMstCD[nMaster_Index][0] = nMaster_codeID;
			m_nMstCD[nMaster_Index][1] = nMasterCEQ_TYPE;
			m_nMstCD[nMaster_Index][2] = nMaster_Circuit;
			nMaster_Index++;
			rs_code.MoveNext();
		}
		rs_code.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[2/15]========== CBSW ON/OFF =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		////CBSW ON/OFF를 찾기위한 정보
		int nMaster_code= 0;
		int nCBSW_Name_type = 0;
		int nCBSW_Ceq_type = 0;
		int nbStatus = 0;
		int nIdx = 0, nMST_CD = 0;
		int nCOUNT = 0;
		CString strNM;
		CString strMRID;
		szADMS_Code.Format(_T("select b.ceq_mrfk, b.master_code_fk, b.value ,(SELECT ceq_type_fk FROM conductingequipment where mrid = b.ceq_mrfk ORDER BY mrid ASC LIMIT 1 ) as ceq_type_fk, (SELECT (select name_type_fk from `%s`.`ceq_type` where ceq_type_id = c.ceq_type_fk  ORDER BY name_type_fk ASC LIMIT 1)  FROM conductingequipment c where c.mrid = b.ceq_mrfk ORDER BY c.mrid ASC LIMIT 1 ) as name_type_fk from bi_value b where value != 0"), m_szADMS_Code);
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, strMRID);
			rs.GetFieldValue((short)1, strData);
			nMaster_code = _wtoi(strData);
			rs.GetFieldValue((short)2, strData);
			nbStatus = _wtoi(strData);
			rs.GetFieldValue((short)3, strData);
			nCBSW_Ceq_type = _wtoi(strData);
			rs.GetFieldValue((short)4, strData);
			nCBSW_Name_type = _wtoi(strData);
			if (nCBSW_Ceq_type)
			{
				if (nCBSW_Name_type == 21 || nCBSW_Name_type == 22 || nCBSW_Name_type == 24 || nCBSW_Name_type == 27)
				{
					nIdx = GetMasterCode(nMaster_code, nCBSW_Ceq_type);
					nMST_CD = GetMasterCD(nCBSW_Ceq_type, nIdx);
				}
				else
				{
					nMST_CD = GetMasterCD(nCBSW_Ceq_type, 1);
				}
				if (nMST_CD == nMaster_code)
				{
					m_map_NEW_CBSW_bStatus.SetAt(strMRID, nbStatus);
				}
			}
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[3/15]========== [HDOF_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		int a = nCOUNT;
		//HDOF_STA
		szADMS_Code.Format(_T("select HEAD_OFFICE_ID, NAME from `%s`.`head_office`;"), m_szADMS_Code);
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szHEAD_ID);
			rs.GetFieldValue((short)1, szHDOF_NM);
			nn1 = 0;
			nn1 = szHDOF_NM.Find(',');
			if (nn1 > -1)
			{
				szHDOF_NM.Replace(_T(","), _T("_"));
			}
			HEAD_STA stHEAD_STA;
			memset(&stHEAD_STA, 0, sizeof(HEAD_STA));
			_stprintf_s(stHEAD_STA.HEAD_ID, _T("%s"), szHEAD_ID);
			_stprintf_s(stHEAD_STA.HEAD_NM, _T("%s"), szHDOF_NM);
			m_arrHEAD_STA.Add(stHEAD_STA);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[4/15]========== [CENTER_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//CENTER_STA
		szADMS_Code.Format(_T("SELECT CENTER_OFFICE_ID, NAME, HEAD_OFFICE_FK FROM `%s`.`center_office`"), m_szADMS_Code);
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szCENTER_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szHEAD_ID);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			CENTER_STA stCENTER;
			memset(&stCENTER, 0, sizeof(CENTER_STA));
			_stprintf_s(stCENTER.CENTER_ID, _T("%s"), szCENTER_MRID);
			_stprintf_s(stCENTER.CENTER_NM, _T("%s"), stNAME);
			_stprintf_s(stCENTER.CENTER_II_HDOF, _T("%s"), szHEAD_ID);
			m_arrCENTER_STA.Add(stCENTER);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[5/15]========== [MEMBER_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//MEMBER_STA
		szADMS_Code.Format(_T("SELECT MEMBER_OFFICE_ID, NAME, CENTER_OFFICE_FK  from `%s`.`member_office`"), m_szADMS_Code);
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szMEMBER_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szCENTER_MRID);
			
			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			MEMBER_STA stMEMBER_STA;
			memset(&stMEMBER_STA, 0, sizeof(MEMBER_STA));
			_stprintf_s(stMEMBER_STA.MEMBER_ID, _T("%s"), szMEMBER_MRID);
			_stprintf_s(stMEMBER_STA.MEMBER_NM, _T("%s"), stNAME);
			_stprintf_s(stMEMBER_STA.MEMBER_II_CENTER, _T("%s"), szCENTER_MRID);
			m_arrMEMBER_STA.Add(stMEMBER_STA);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[6/15]========== [SS_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//SS_STA 
		szADMS_Code.Format(_T("SELECT A.EQC_MRFK, B.NAME ,A.SUBSTATION_CODE, A.HEAD_OFFICE_FK FROM substation A, identifiedobject B WHERE A.EQC_MRFK = B.MRID"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szSS_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szSS_CODE);
			rs.GetFieldValue((short)3, szHEAD_ID);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			if (szSS_CODE.IsEmpty())
			{
				szSS_CODE.Format(_T("NULL"));
			}

			SS_STA stSS;
			memset(&stSS, 0, sizeof(SS_STA));
			_stprintf_s(stSS.SS_ID, _T("%s"), szSS_MRID);
			_stprintf_s(stSS.SS_NM, _T("%s"), stNAME);
			_stprintf_s(stSS.SS_CODE, _T("%s"), szSS_CODE);
			_stprintf_s(stSS.SS_II_HEAD, _T("%s"), szHEAD_ID);
			NEW_SUBSTATION_INSERT(m_arrSS_STA.GetSize()+1, stNAME); //신규 변전소쪽 발전기 만들기 부분!!!
			m_arrSS_STA.Add(stSS);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[7/15]========== [MTR_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//MTR_STA
		szADMS_Code.Format(_T("SELECT PA.CEQ_MRFK, PI.NAME, PA.BANK_NO, (SELECT SUBS_MRFK FROM voltagelevel WHERE EQC_MRFK = PA.SECONDARY_VOLTAGELEVEL_FK) AS SUBSID FROM powertransformer PA LEFT JOIN identifiedobject PI ON PA.CEQ_MRFK = PI.MRID AND PI.NAME_TYPE_FK = 17"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szMTR_MRID);
			rs.GetFieldValue((short)1, stNAME);		
			rs.GetFieldValue((short)2, strData);
			nMTR_BANK_NO = _wtoi(strData);
			rs.GetFieldValue((short)3, szSS_MRID);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}

			NEW_MTR_INSERT(szMTR_MRID, stNAME, m_arrMTR_STA.GetSize()+1);
			//MTR은 ND 를 만들어서 넣는 부분
			szFND_CN.Format(_T("999917%08d00"), m_arrMTR_STA.GetSize() + 1);
			szTND_CN.Format(_T("999915%08d10"), m_arrMTR_STA.GetSize() + 1);

			MTR_STA stMTR;
			memset(&stMTR, 0, sizeof(MTR_STA));
			_stprintf_s(stMTR.MTR_ID, L"%s", szMTR_MRID);
			_stprintf_s(stMTR.MTR_NM, L"%s", stNAME);
			_stprintf_s(stMTR.MTR_II_SS, L"%s", szSS_MRID);
			stMTR.MTR_BANK = nMTR_BANK_NO;
			m_arrMTR_STA.Add(stMTR);

			//TR
			TR_STA stTR;
			memset(&stTR, 0, sizeof(TR_STA));
			_stprintf_s(stTR.TR_ID, L"%s", szMTR_MRID);
			_stprintf_s(stTR.TR_NM, L"%s", stNAME);
			_stprintf_s(stTR.TR_II_FND, L"%s", szFND_CN);
			_stprintf_s(stTR.TR_II_TND, L"%s", szTND_CN);
			stTR.TR_TYPE = 1;											 //MTR 은1번입니다.
			stTR.TR_FNORKV = 154;
			stTR.TR_TNORKV = 22.9;
			stTR.TR_POSX = 30;
			stTR.TR_ZERX = 30;
			_stprintf_s(stTR.TR_II_MTR, L"%s", szMTR_MRID);
			m_arrTR_STA.Add(stTR);
			rs.MoveNext(); //다음행으로 이동
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[8/15]========== [DL_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//DL_STA
		szADMS_Code.Format(_T("SELECT PDL.EQC_MRFK, PI.NAME, PDL.MEMBER_OFFICE_FK, PDL.MTR_MRFK, PDL.CEQ_MRFK FROM DL PDL LEFT JOIN identifiedobject PI ON PDL.EQC_MRFK = PI.MRID AND PI.NAME_TYPE_FK = 13"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szDL_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szMEMBER_MRID);
			rs.GetFieldValue((short)3, szMTR_MRID);
			rs.GetFieldValue((short)4, szDL_CB_MRID);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			DL_STA stDl;
			memset(&stDl, 0, sizeof(DL_STA));
			_stprintf_s(stDl.DL_ID, L"%s", szDL_MRID);
			_stprintf_s(stDl.DL_NM, L"%s", stNAME);
			_stprintf_s(stDl.DL_II_MEMBER, L"%s", szMEMBER_MRID);
			_stprintf_s(stDl.DL_II_MTR, L"%s", szMTR_MRID);
			_stprintf_s(stDl.DL_II_CB, L"%s", szDL_CB_MRID);
			m_arrDL_STA.Add(stDl);

			m_map_NEW_CBMRID_MTRMRID.SetAt(szDL_CB_MRID, szMTR_MRID);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[9/15]========== [ND_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//ND_STA
		szADMS_Code.Format(_T("SELECT ND_ID, ND_NM FROM V_ND_STA;"));
		if (rs_Application.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs_Application.IsEOF())
		{
			rs_Application.GetFieldValue((short)0, szND_MRID);
			rs_Application.GetFieldValue((short)1, stNAME);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			ND_STA stNd;
			memset(&stNd, 0, sizeof(ND_STA));
			_stprintf_s(stNd.ND_ID, _T("%s"), szND_MRID);
			_stprintf_s(stNd.ND_NM, _T("%s"), stNAME);
			m_arrND_STA.Add(stNd);
			rs_Application.MoveNext();
		}
		rs_Application.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[10/15]========== [CBSW_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//CBSW_STA
		int nCbsw_RTUTYPE = 0;
		szADMS_Code.Format(_T("select CBSW_ID, CEQ_TYPE_FK, CBSW_MULTISW_ID, CBSW_MULTICIR_NUMBER, CBSW_NM, CBSW_COMTYPE,CBSW_RTUCODE, CBSW_II_FND, CBSW_II_TND from v_cbsw_sta;"));
		if (rs_Application.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs_Application.IsEOF())
		{
			rs_Application.GetFieldValue((short)0, szCBSW_MRID);
			rs_Application.GetFieldValue((short)1, strData);
			nCEQ_TYPE = _wtoi(strData);
			rs_Application.GetFieldValue((short)2, szMUCBSW_MRID);
			rs_Application.GetFieldValue((short)3, strData);
			nMU_Munber = _wtoi(strData);
			rs_Application.GetFieldValue((short)4, stNAME);
			rs_Application.GetFieldValue((short)5, strData);
			nCbsw_ComType = _wtoi(strData);		
			rs_Application.GetFieldValue((short)6, strData);
			nCbsw_RTUCODE = _wtoi(strData);
			rs_Application.GetFieldValue((short)7, szFND_CN);
			rs_Application.GetFieldValue((short)8, szTND_CN);

			nContinue = 0;
			nbStatus = 0;
			nContinue = NEW_CBSW_MAKE_NO(nCEQ_TYPE, nMU_Munber); //이부분은 만들지 않아도 되는 CBSW입니다.
			if (nContinue == 9999)		
			{			
				rs_Application.MoveNext();
				continue;
			} 

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}

			if (szMUCBSW_MRID.IsEmpty()) {	szMUCBSW_MRID.Format(_T("0"));			}
			if (szFND_CN.IsEmpty())	{	szFND_CN.Format(_T("99%s"), szCBSW_MRID);	}
			if (szTND_CN.IsEmpty())	{	szTND_CN.Format(_T("99%s"), szCBSW_MRID);	}

			//CBSW타입 CBSW_TYPE변환
			nCbsw_RTUTYPE = nCEQ_TYPE;
			nCBSW_TYPE = NEW_CBSW_TYPE(nCEQ_TYPE, nMU_Munber);
			//CBSW ON/OFF MAP정보
			m_map_NEW_CBSW_bStatus.Lookup(szCBSW_MRID, nbStatus); 
			if (nbStatus == 1)
			{
				nbStatus = 0;
			}
			else
			{
				nbStatus = 1;
			}
			//
			CBSW_STA stCbsw;
			memset(&stCbsw, 0, sizeof(CBSW_STA));
			_stprintf_s(stCbsw.CBSW_ID, L"%s", szCBSW_MRID);
			_stprintf_s(stCbsw.CBSW_NM, L"%s", stNAME);
			stCbsw.CBSW_TYPE = nCBSW_TYPE;
			stCbsw.CBSW_RTUTYPE = nCbsw_RTUTYPE;
			stCbsw.CBSW_RTUCODE = nCbsw_RTUCODE;
			stCbsw.CBSW_COMTYPE = nCbsw_ComType;
			stCbsw.CBSW_NORSTAT = nbStatus;
			_stprintf_s(stCbsw.CBSW_II_FND, L"%s", szFND_CN);
			_stprintf_s(stCbsw.CBSW_II_TND, L"%s", szTND_CN);
			_stprintf_s(stCbsw.CBSW_MULTISW_ID, L"%s", szMUCBSW_MRID);
			stCbsw.CBSW_MULTICIR_NUMBER = nMU_Munber;
			m_arrCBSW_STA.Add(stCbsw);
			
			rs_Application.MoveNext();
		}
		rs_Application.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[11/15]========== [GEN_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//GEN_STA 
		CString szGEN_MRID;
		int nGEN_TYPE = 0;
		szADMS_Code.Format(_T("SELECT  AA.MRID, AA.NAME, AA.CONNECTIVITYNODE_FK, BB.ENERGYSOURCE_TYPE_FK  FROM (SELECT PI.MRID, PI.NAME, PTER.CONNECTIVITYNODE_FK FROM identifiedobject PI LEFT JOIN terminal PTER ON PI.MRID = PTER.CEQ_MRFK WHERE PI.NAME_TYPE_FK = 37) AA LEFT JOIN (SELECT DISTINCT CONNECT_CEQ_MRFK,ENERGYSOURCE_TYPE_FK FROM energysource) BB ON AA.MRID = BB.CONNECT_CEQ_MRFK GROUP BY AA.MRID;"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szGEN_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szND_MRID);
			rs.GetFieldValue((short)3, strData);
			nGEN_TYPE = _wtoi(strData);

			if (szND_MRID.IsEmpty())
			{
				m_map_NEW_GEN_NDisnull.SetAt(szGEN_MRID, 9999);
				rs.MoveNext();
			}
			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			//2021 0728 심재성 차장이 메일로 보내줌 
			if (nGEN_TYPE == 1) { nGEN_TYPE = 5; }
			else  if (nGEN_TYPE == 2) { nGEN_TYPE = 4; }
			else  if (nGEN_TYPE == 3) { nGEN_TYPE = 6; }
			else  if (nGEN_TYPE == 4) { nGEN_TYPE = 7; }
			else  if (nGEN_TYPE == 5) { nGEN_TYPE = 8; }
			else  if (nGEN_TYPE == 6) { nGEN_TYPE = 3; }
			else  if (nGEN_TYPE == 7) { nGEN_TYPE = 9; }
			else  if (nGEN_TYPE == 100) { nGEN_TYPE = 10; }
			else { nGEN_TYPE = 5; }

			GEN_STA stGEN;
			memset(&stGEN, 0, sizeof(GEN_STA));
			_stprintf_s(stGEN.GEN_ID, L"%s", szGEN_MRID);
			_stprintf_s(stGEN.GEN_NM, L"%s", stNAME);
			_stprintf_s(stGEN.GEN_II_ND, L"%s", szND_MRID);
			stGEN.GEN_TYPE = nGEN_TYPE;
			m_arrGEN_STA.Add(stGEN);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[12/15]========== [GENUNIT_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);
		
		int nCheck = 0;
		//GENUNIT_STA
		szADMS_Code.Format(_T("select AA.ENERGYSOURCE_ID, AA.CUSTOMER_NAME,AA.CONNECT_CEQ_MRFK, AA.CONTRACT_CAPACITY,AA.ENERGYSOURCE_TYPE_FK, AA.CONTRACT_TYPE, AA.CONTRACT_STATUS from energysource AA INNER JOIN identifiedobject BB ON AA.CONNECT_CEQ_MRFK = BB.MRID ORDER BY BB.NAME_TYPE_FK ;"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szGENUNIT_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szND_MRID);
			rs.GetFieldValue((short)3, strData);
			fGENUNIT_CAPACITY = _wtof(strData);
			rs.GetFieldValue((short)4, strData);
			nGENUNIT_TYPE = _wtoi(strData);
			rs.GetFieldValue((short)5, strData);
			nGENUNIT_CONTRACT_TYPE = _wtoi(strData);
			rs.GetFieldValue((short)6, strData);
			nGENUNIT_CONTRACT_STATUS = _wtoi(strData);

			nCheck = 0;
			m_map_NEW_GEN_NDisnull.Lookup(szND_MRID, nCheck);
			if (nCheck == 9999)
			{
				rs.MoveNext();
			}
			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			GENUNIT_STA stGENUNIT;
			memset(&stGENUNIT, 0, sizeof(GENUNIT_STA));
			_stprintf_s(stGENUNIT.GENUNIT_ID, L"%s", szGENUNIT_MRID);
			_stprintf_s(stGENUNIT.GENUNIT_NM, L"%s", stNAME);
			_stprintf_s(stGENUNIT.GENUNIT_CONNECT_CEQID, L"%s", szND_MRID);
			stGENUNIT.GENUNIT_CAP_KW = fGENUNIT_CAPACITY;
			stGENUNIT.GENUNIT_TYPE = nGENUNIT_TYPE;
			stGENUNIT.GENUNIT_CONTRACT_TYPE = nGENUNIT_CONTRACT_TYPE;
			stGENUNIT.GENUNIT_CONTRACT_STATUS = nGENUNIT_CONTRACT_STATUS;
			m_arrGENUNIT_STA.Add(stGENUNIT);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[13/15]========== [HVCUS_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//HVCUS_STA  
		szADMS_Code.Format(_T("SELECT PEC.CEQ_MRFK, PI.NAME,  (SELECT  CONNECTIVITYNODE_FK  FROM terminal WHERE  CEQ_MRFK = PEC.CEQ_MRFK ORDER BY CONNECTIVITYNODE_FK DESC LIMIT 1 ) AS CN,  PEC.PROMISE_LOAD FROM energyconsumer PEC LEFT JOIN identifiedobject PI ON PEC.CEQ_MRFK = PI.MRID WHERE PI.NAME_TYPE_FK = 28"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szHVCUS_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szND_MRID);
			rs.GetFieldValue((short)3, strData);
			nHVCUS_CON_KVA = _wtof(strData);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			HVCUS_STA stHVCUS;
			memset(&stHVCUS, 0, sizeof(HVCUS_STA));
			_stprintf_s(stHVCUS.HVCUS_ID, L"%s", szHVCUS_MRID);
			_stprintf_s(stHVCUS.HVCUS_NM, L"%s", stNAME);
			_stprintf_s(stHVCUS.HVCUS_II_ND, L"%s", szND_MRID);
			stHVCUS.HVCUS_CON_KVA = nHVCUS_CON_KVA;
			m_arrHVCUS_STA.Add(stHVCUS);
			rs.MoveNext();
		}
		rs.Close();

		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[14/15]========== [LNSEC_STA] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		float fPOSITIVE_R, fPOSITIVE_X, fZERO_R, fZERO_X;
		//LNSEC_STA
		szADMS_Code.Format(_T("SELECT  LNSEC_MRID, LNSEC_NAME_TYPE, LNSEC_SET_TYPE, LNSEC_NM, LNSEC_LENGTH, LNSEC_FND, LNSEC_TND, POSITIVE_R, POSITIVE_X, ZERO_R, ZERO_X  FROM V_LNSEC_STA"));
		if (rs_Application.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs_Application.IsEOF())
		{
			rs_Application.GetFieldValue((short)0, szLNSEC_MRID);
			rs_Application.GetFieldValue((short)1, strData);
			nNAME_TYPE = _wtoi(strData);
			rs_Application.GetFieldValue((short)2, strData);
			nLNSEC_TYPE = _wtoi(strData);
			rs_Application.GetFieldValue((short)3, stNAME);
			rs_Application.GetFieldValue((short)4, strData);
			fLNSEC_LENGTH = (float)_wtof(strData);
			rs_Application.GetFieldValue((short)5, szFND_CN);
			rs_Application.GetFieldValue((short)6, szTND_CN);
			rs_Application.GetFieldValue((short)7, strData);
			fPOSITIVE_R = (float)_wtof(strData);
			rs_Application.GetFieldValue((short)8, strData);
			fPOSITIVE_X = (float)_wtof(strData);
			rs_Application.GetFieldValue((short)9, strData);
			fZERO_R = (float)_wtof(strData);
			rs_Application.GetFieldValue((short)10, strData);
			fZERO_X = (float)_wtof(strData);

			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}

			LNSEC_STA stLnsec;
			memset(&stLnsec, 0, sizeof(LNSEC_STA));
			_stprintf_s(stLnsec.LNSEC_ID, L"%s", szLNSEC_MRID);
			_stprintf_s(stLnsec.LNSEC_NM, L"%s", stNAME);
			_stprintf_s(stLnsec.LNSEC_II_FND, L"%s", szFND_CN);
			_stprintf_s(stLnsec.LNSEC_II_TND, L"%s", szTND_CN);
			stLnsec.LNSEC_POSR = fLNSEC_LENGTH * fPOSITIVE_R;
			stLnsec.LNSEC_POSX = fLNSEC_LENGTH * fPOSITIVE_X;
			stLnsec.LNSEC_ZERR = fLNSEC_LENGTH * fZERO_R;
			stLnsec.LNSEC_ZERX = fLNSEC_LENGTH * fZERO_X;
			stLnsec.LNSEC_THRLM = 0;
			stLnsec.LNSEC_LENGTH = fLNSEC_LENGTH;
			m_arrLNSEC_STA.Add(stLnsec);

			//LD
			if (nNAME_TYPE == 51)
			{
				LD_STA stLd;
				memset(&stLd, 0, sizeof(LD_STA));
				_stprintf_s(stLd.LD_ID, L"%s", szLNSEC_MRID);
				_stprintf_s(stLd.LD_NM, L"%s", stNAME);
				_stprintf_s(stLd.LD_II_ND, L"%s", szTND_CN);
				m_arrLD_STA.Add(stLd);
			}
			rs_Application.MoveNext();
		}
		rs_Application.Close();
			   		 
		m_szTime = LIST_Current_Time();
		m_szDataName_Data.Format(_T("%s[15/15]========== [SVR] =========="), m_szTime);
		IDC_LIST_DATA_HISTORY(m_szDataName_Data);

		//SVR
		szADMS_Code.Format(_T("select aa.MRID, AA.NAME,(select mtr_mrfk from dl where eqc_mrfk in (select change_eqc_mrfk from conductingequipment where mrid = aa.mrid)) as MTRMRFK , (SELECT  CONNECTIVITYNODE_FK  FROM `adms_office`.`terminal` WHERE  CEQ_MRFK = aa.MRID  ORDER BY CONNECTIVITYNODE_FK ASC LIMIT 1 ) AS CBSW_II_FND,  (SELECT  CONNECTIVITYNODE_FK  FROM `adms_office`.`terminal` WHERE  CEQ_MRFK = aa.MRID  ORDER BY CONNECTIVITYNODE_FK DESC LIMIT 1 ) AS CBSW_II_TND from identifiedobject aa, conductingequipment cc where aa.MRID = cc.MRID and cc.CEQ_TYPE_FK = 81"));
		if (rs.Open(CRecordset::snapshot, szADMS_Code, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, szTR_MRID);
			rs.GetFieldValue((short)1, stNAME);
			rs.GetFieldValue((short)2, szMTR_MRID);
			rs.GetFieldValue((short)3, szFND_CN);
			rs.GetFieldValue((short)4, szTND_CN);


			nn1 = 0;
			nn1 = stNAME.Find(',');
			if (nn1 > -1)
			{
				stNAME.Replace(_T(","), _T("_"));
			}
			//TR
			TR_STA stTR;
			memset(&stTR, 0, sizeof(TR_STA));
			_stprintf_s(stTR.TR_ID, L"%s", szTR_MRID);
			_stprintf_s(stTR.TR_NM, L"%s", stNAME);
			_stprintf_s(stTR.TR_II_FND, L"%s", szFND_CN);
			_stprintf_s(stTR.TR_II_TND, L"%s", szTND_CN);
			stTR.TR_TYPE = 1;											 //MTR 은1번입니다.
			stTR.TR_FNORKV = 10;
			stTR.TR_TNORKV = 10;
			stTR.TR_POSX = 3.6;
			stTR.TR_ZERX = 3.6;
			_stprintf_s(stTR.TR_II_MTR, L"%s", szMTR_MRID);
			m_arrTR_STA.Add(stTR);
			rs.MoveNext();
		}
		rs.Close();		
	}
	catch (CDBException * e)
	{
		//AfxMessageBox(e->m_strError);
		_tprintf(_T("%s"), e->m_strError.GetBuffer());
		return;
	}
}

////////////////////////////////////////
void CADMStoKCIMDlg::DeleteAllFiles(CString dirName)
{
	CFileFind finder;

	BOOL bWorking = finder.FindFile((CString)dirName + "/*.*");

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}

		CString filePath = finder.GetFilePath();
		DeleteFile(filePath);
	}
	finder.Close();
}

void CADMStoKCIMDlg::PumpMessages() //변환된 데이터를 변경을 하기 위한 메세지 함수
{
	ASSERT(m_hWnd != NULL);

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CADMStoKCIMDlg::IDC_LIST_DATA_HISTORY(CString  strData_Name)
{
	CListBox *ListBox = (CListBox*)GetDlgItem(IDC_LIST_ADMStoKCIM);
	ListBox->InsertString(ListBox->GetCount(), strData_Name);
	PumpMessages();
	m_ListCtrl_Data.SetCurSel(m_ListCtrl_Data.GetCount() - 1);
}

int CADMStoKCIMDlg::GetMasterCD(int nCeqTp, int nCircuit)
{
	for (int i = 0; i < 500; i++)
	{
		if (nCeqTp == m_nMstCD[i][1] && nCircuit == m_nMstCD[i][2]) return m_nMstCD[i][0];
	}
	return 9999;
}

int CADMStoKCIMDlg::GetMasterCode(int nMst, int nCeqTp)
{
	CString strSQL, strData;
	CRecordset rs(&m_ADMSCDDB);

	strSQL.Format(L"select circuit from master_code where master_code_id='%d' and ceq_type_fk='%d'", nMst, nCeqTp);
	try
	{

		if (rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly) == FALSE)
			AfxMessageBox(L"에러!");
		if (rs.GetRecordCount())
		{
			rs.GetFieldValue((short)0, strData);
		}
		rs.Close();


		return _wtoi(strData);
	}
	catch (CDBException * e)
	{
		_tprintf(_T("%s"), e->m_strError.GetBuffer());
		return 0;
	}
	return 0;
}

//20220902 - 변전소 생성 부분중 LNSEC연결된 양쪽 노드 까지만 생성하는 부분 
void CADMStoKCIMDlg::NEW_SUBSTATION_INSERT(int nSS_ID, CString stSS_NM)
{
	CString stLNSEC_TND;

	ND_STA stNd;
	memset(&stNd, 0, sizeof(ND_STA));
	_stprintf_s(stNd.ND_NM, _T("%s"), stSS_NM);
	_stprintf_s(stNd.ND_ID, _T("999961%08d00"), nSS_ID);
	m_arrND_STA.Add(stNd);

	GEN_STA stGEN;
	memset(&stGEN, 0, sizeof(GEN_STA));
	_stprintf_s(stGEN.GEN_ID, _T("999961%08d"), nSS_ID);
	_stprintf_s(stGEN.GEN_NM, _T("%s"), stSS_NM);
	_stprintf_s(stGEN.GEN_II_ND, _T("999961%08d00"), nSS_ID);
	stGEN.GEN_TYPE = 1;
	m_arrGEN_STA.Add(stGEN);
	//선로
	ND_STA stNd_LNSEC;
	memset(&stNd_LNSEC, 0, sizeof(ND_STA));
	_stprintf_s(stNd_LNSEC.ND_NM, _T("%s-SS"), stSS_NM);
	_stprintf_s(stNd_LNSEC.ND_ID, _T("999922%08d00"), nSS_ID);
	m_arrND_STA.Add(stNd_LNSEC);

	//LNSEC
	LNSEC_STA stLnsec;
	memset(&stLnsec, 0, sizeof(LNSEC_STA));
	_stprintf_s(stLnsec.LNSEC_ID, _T("999922%08d"), nSS_ID);
	_stprintf_s(stLnsec.LNSEC_NM, _T("%s-SS"), stSS_NM);
	_stprintf_s(stLnsec.LNSEC_II_FND, _T("999961%08d00"), nSS_ID);
	_stprintf_s(stLnsec.LNSEC_II_TND, _T("999922%08d00"), nSS_ID);
	stLnsec.LNSEC_POSR = 0;
	stLnsec.LNSEC_POSX = 0;
	stLnsec.LNSEC_ZERR = 0;
	stLnsec.LNSEC_ZERX = 0;
	stLnsec.LNSEC_THRLM = 0;
	stLnsec.LNSEC_LENGTH = 0.001;
	m_arrLNSEC_STA.Add(stLnsec);
}

void CADMStoKCIMDlg::NEW_MTR_INSERT(CString strMRID, CString stNAME, int nMTR_ID)
{
	ND_STA stNd_MTR;
	memset(&stNd_MTR, 0, sizeof(ND_STA));
	_stprintf_s(stNd_MTR.ND_NM, _T("%sMTR"), stNAME);
	_stprintf_s(stNd_MTR.ND_ID, _T("999917%08d00"), nMTR_ID);
	m_arrND_STA.Add(stNd_MTR);

	ND_STA stNd_CBSW1;
	memset(&stNd_CBSW1, 0, sizeof(ND_STA));
	_stprintf_s(stNd_CBSW1.ND_NM, _T("%sDS-1"), stNAME);
	_stprintf_s(stNd_CBSW1.ND_ID, _T("999915%08d10"), nMTR_ID);
	m_arrND_STA.Add(stNd_CBSW1);

	ND_STA stNd_CBSW2;
	memset(&stNd_CBSW2, 0, sizeof(ND_STA));
	_stprintf_s(stNd_CBSW2.ND_NM, _T("%sDS-2"), stNAME);
	_stprintf_s(stNd_CBSW2.ND_ID, _T("999915%08d20"), nMTR_ID);
	m_arrND_STA.Add(stNd_CBSW2);

	CBSW_STA stCbsw1;
	memset(&stCbsw1, 0, sizeof(CBSW_STA));
	_stprintf_s(stCbsw1.CBSW_NM, L"%sDS-1", stNAME);
	_stprintf_s(stCbsw1.CBSW_ID, L"99991501%06d", nMTR_ID);
	stCbsw1.CBSW_TYPE = 4;
	stCbsw1.CBSW_RTUTYPE = 0;
	stCbsw1.CBSW_RTUCODE = 0;
	stCbsw1.CBSW_COMTYPE = 0;
	stCbsw1.CBSW_NORSTAT = 1;
	_stprintf_s(stCbsw1.CBSW_II_FND, _T("999917%08d00"), nMTR_ID);
	_stprintf_s(stCbsw1.CBSW_II_TND, _T("999915%08d10"), nMTR_ID);
	_stprintf_s(stCbsw1.CBSW_MULTISW_ID, L"0");
	stCbsw1.CBSW_MULTICIR_NUMBER = 0;
	m_arrCBSW_STA.Add(stCbsw1);


	CBSW_STA stCbsw2;
	memset(&stCbsw2, 0, sizeof(CBSW_STA));
	_stprintf_s(stCbsw2.CBSW_NM, L"%sDS-2", stNAME);
	_stprintf_s(stCbsw2.CBSW_ID, L"99991502%06d", nMTR_ID);
	stCbsw2.CBSW_TYPE = 4;
	stCbsw2.CBSW_RTUTYPE = 0;
	stCbsw2.CBSW_RTUCODE = 0;
	stCbsw2.CBSW_COMTYPE = 0;
	stCbsw2.CBSW_NORSTAT = 1;
	_stprintf_s(stCbsw2.CBSW_II_FND, _T("999917%08d00"), nMTR_ID);
	_stprintf_s(stCbsw2.CBSW_II_TND, _T("999915%08d20"), nMTR_ID);
	_stprintf_s(stCbsw2.CBSW_MULTISW_ID, L"0");
	stCbsw2.CBSW_MULTICIR_NUMBER = 0;
	m_arrCBSW_STA.Add(stCbsw2);

	CString stCBSWFDS_CN1, stCBSWFDS_CN2;
	stCBSWFDS_CN1.Format(_T("999915%08d10"), nMTR_ID);
	stCBSWFDS_CN2.Format(_T("999915%08d20"), nMTR_ID);

	m_map_NEW_DS1_CBSW_CEQ_ND.SetAt(strMRID, stCBSWFDS_CN1);
	m_map_NEW_DS2_CBSW_CEQ_ND.SetAt(strMRID, stCBSWFDS_CN2);
}

void CADMStoKCIMDlg::NEW_DS_OCB()
{
	int i = 0;
	CString szCBSW_NM, szCBSW_CEQ;
	CString szMTR_MRID;
	CString szCB1_ND, szCB2_ND;


	for (i = 0; i < m_arrCBSW_STA.GetSize(); i++)
	{
		if (m_arrCBSW_STA[i].CBSW_TYPE == 1) //CB 가 1번으로 들어갑니다.
		{
			szCBSW_CEQ.Format(_T("%s"), m_arrCBSW_STA[i].CBSW_ID);
			szCBSW_NM.Format(_T("%s"), m_arrCBSW_STA[i].CBSW_NM);
			//DL에서 CB에 MTR입력
			m_map_NEW_CBMRID_MTRMRID.Lookup(szCBSW_CEQ, szMTR_MRID);

			m_map_NEW_DS1_CBSW_CEQ_ND.Lookup(szMTR_MRID, szCB1_ND);
			m_map_NEW_DS2_CBSW_CEQ_ND.Lookup(szMTR_MRID, szCB2_ND);

			CBSW_STA stCbsw1;
			memset(&stCbsw1, 0, sizeof(CBSW_STA));
			_stprintf_s(stCbsw1.CBSW_NM, L"%s-1", szCBSW_NM);
			_stprintf_s(stCbsw1.CBSW_ID, L"99991401%06d", i);
			stCbsw1.CBSW_TYPE = 4;
			stCbsw1.CBSW_RTUTYPE = 0;
			stCbsw1.CBSW_RTUCODE = 0;
			stCbsw1.CBSW_COMTYPE = 0;
			stCbsw1.CBSW_NORSTAT = 1;
			_stprintf_s(stCbsw1.CBSW_II_FND, L"%s", szCB1_ND);
			_stprintf_s(stCbsw1.CBSW_II_TND, L"%s", m_arrCBSW_STA[i].CBSW_II_FND);
			_stprintf_s(stCbsw1.CBSW_MULTISW_ID, L"0");
			stCbsw1.CBSW_MULTICIR_NUMBER = 0;
			m_arrCBSW_STA.Add(stCbsw1);

			CBSW_STA stCbsw2;
			memset(&stCbsw2, 0, sizeof(CBSW_STA));
			_stprintf_s(stCbsw2.CBSW_NM, L"%s-2", szCBSW_NM);
			_stprintf_s(stCbsw2.CBSW_ID, L"99991402%06d", i);
			stCbsw2.CBSW_TYPE = 4;
			stCbsw2.CBSW_RTUTYPE = 0;
			stCbsw2.CBSW_RTUCODE = 0;
			stCbsw2.CBSW_COMTYPE = 0;
			stCbsw2.CBSW_NORSTAT = 1;
			_stprintf_s(stCbsw2.CBSW_II_FND, L"%s", szCB2_ND);
			_stprintf_s(stCbsw2.CBSW_II_TND, L"%s", m_arrCBSW_STA[i].CBSW_II_FND);
			_stprintf_s(stCbsw2.CBSW_MULTISW_ID, L"0");
			stCbsw2.CBSW_MULTICIR_NUMBER = 0;
			m_arrCBSW_STA.Add(stCbsw2);
		}
	}
}

int CADMStoKCIMDlg::NEW_CBSW_MAKE_NO(int nCeqType, int nMULTICIR_NUMBER)
{
	if (nCeqType == 53 || nCeqType == 54 || nCeqType == 57)
	{
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 2)
		{
			return 9999;
		}
	}
	if (nCeqType == 83)
	{
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 2 || nMULTICIR_NUMBER == 3 || nMULTICIR_NUMBER == 4)
		{
			return 9999;
		}
	}
	if (nCeqType == 84)
	{
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 2 || nMULTICIR_NUMBER == 3 || nMULTICIR_NUMBER == 4 || nMULTICIR_NUMBER == 5 || nMULTICIR_NUMBER == 6)
		{
			return 9999;
		}
	}
	if (nCeqType == 85 || nCeqType == 86 || nCeqType == 87 || nCeqType == 88 || nCeqType == 89)
	{
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 2 || nMULTICIR_NUMBER == 3 || nMULTICIR_NUMBER == 4 || nMULTICIR_NUMBER == 5)
		{
			return 9999;
		}
	}
	return 0;
}

int CADMStoKCIMDlg::NEW_CBSW_MULTICIR_NUMBER_TYPE_CHANG(int nCeqType, int nMULTICIR_NUMBER)
{
	if (nCeqType == 53 || nCeqType  == 54 || nCeqType == 57)
	{
		if (nMULTICIR_NUMBER == 3) { return 1; }
		if (nMULTICIR_NUMBER == 4) { return 2; }
	}
	if (nCeqType == 83)
	{
		if (nMULTICIR_NUMBER == 5) { return 1; }
		if (nMULTICIR_NUMBER == 6) { return 2; }
	}
	if (nCeqType == 84)
	{
		if (nMULTICIR_NUMBER == 7) { return 1; }
		if (nMULTICIR_NUMBER == 8) { return 2; }
	}
	if (nCeqType == 85 || nCeqType == 86 || nCeqType == 87 || nCeqType == 88 || nCeqType == 89)
	{
		if (nMULTICIR_NUMBER == 6) { return 1; }
		if (nMULTICIR_NUMBER == 7) { return 2; }
	}
	return 0;
}

int CADMStoKCIMDlg::NEW_CBSW_TYPE(int nCeqType, int nMULTICIR_NUMBER)
{
	if (nCeqType == 71 || nCeqType == 207)
	{	//1. 차단기
		return 1;
	}
	else if (nCeqType == 58 || nCeqType == 59 || nCeqType == 60 || nCeqType == 61 || nCeqType == 62 ||
		nCeqType == 63 || nCeqType == 64 || nCeqType == 65 || nCeqType == 66 || nCeqType == 67 ||
		nCeqType == 110)
	{ // 2.리클로저
		return 2;
	}
	else if (nCeqType == 1 || nCeqType == 2 || nCeqType == 4 || nCeqType == 7 || nCeqType == 9 ||
		nCeqType == 10 || nCeqType == 11 || nCeqType == 12 || nCeqType == 14 || nCeqType == 15 ||
		nCeqType == 16 || nCeqType == 19 || nCeqType == 20 || nCeqType == 21 || nCeqType == 24 ||
		nCeqType == 25 || nCeqType == 26 || nCeqType == 28 || nCeqType == 29 || nCeqType == 31 ||
		nCeqType == 33 || nCeqType == 34 || nCeqType == 35 || nCeqType == 36 || nCeqType == 37 ||
		nCeqType == 38 || nCeqType == 39 || nCeqType == 48 || nCeqType == 49 || nCeqType == 50 ||
		nCeqType == 51 || nCeqType == 52 || nCeqType == 72 || nCeqType == 320 || nCeqType == 321 ||
		nCeqType == 322 || nCeqType == 323 || nCeqType == 324 || nCeqType == 325 || nCeqType == 326 ||
		nCeqType == 329 || nCeqType == 330)
	{ // 3.자동 || nCeqType == 73 || nCeqType == 79
		return 3;
	}
	else if (nCeqType == 3 || nCeqType == 5 || nCeqType == 6 || nCeqType == 8 || nCeqType == 13 || nCeqType == 23 || nCeqType == 27 || nCeqType == 30 || nCeqType == 32 ||
		nCeqType == 40 || nCeqType == 41 || nCeqType == 42 || nCeqType == 43 || nCeqType == 44 || nCeqType == 55 || nCeqType == 56 || nCeqType == 121 || nCeqType == 206)
	{ // 4.수동
		return 4;
	}

	else if (nCeqType == 68 || nCeqType == 70 || nCeqType == 74 || nCeqType == 76 || nCeqType == 77 || nCeqType == 78)
	{ // 5.퓨즈
		return 5;
	}
	else if (nCeqType == 69)
	{ // 
		return 6;
	}
	else if (nCeqType == 18 || nCeqType == 314 || nCeqType == 327)
	{ // 7. 저압
		return 7;
	}
	else if (nCeqType == 17 || nCeqType == 22 || nCeqType == 45 || nCeqType == 46 || nCeqType == 47 || nCeqType == 313)
	{ // 8. 고압
		return 8;
	}
	else if (nCeqType == 53 || nCeqType == 54 || nCeqType == 57 || nCeqType == 82 || nCeqType == 83 || nCeqType == 84 || nCeqType == 85 || nCeqType == 86 ||
		nCeqType == 87 || nCeqType == 88 || nCeqType == 89 || nCeqType == 114 || nCeqType == 115)
	{ // 8. 고압
		return 9;
	}
	else if (nCeqType == 79 || nCeqType == 80)
	{
		return 10;
	}
	else if (nCeqType == 328) {
		return 12;
	}
	else if (nCeqType == 75) {
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 4) {
			return 3;
		}
		if (nMULTICIR_NUMBER == 2 || nMULTICIR_NUMBER == 3) {
			return 5;
		}
	}
	else if (nCeqType == 301 || nCeqType == 302 || nCeqType == 303) {
		if (nMULTICIR_NUMBER == 1) {
			return 3;
		}
		if (nMULTICIR_NUMBER == 2 || nMULTICIR_NUMBER == 3 || nMULTICIR_NUMBER == 4) {
			return 5;
		}
	}
	else if (nCeqType == 312 || nCeqType == 315 || nCeqType == 316) {
		if (nMULTICIR_NUMBER == 1) {
			return 3;
		}
		if (nMULTICIR_NUMBER == 2 || nMULTICIR_NUMBER == 3) {
			return 5;
		}
	}
	else if (nCeqType == 304 || nCeqType == 305) {
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 2) {
			return 3;
		}
		if (nMULTICIR_NUMBER == 3 || nMULTICIR_NUMBER == 4) {
			return 5;
		}
	}
	else if (nCeqType == 317 || nCeqType == 318 || nCeqType == 319) {
		if (nMULTICIR_NUMBER == 1 || nMULTICIR_NUMBER == 2) {
			return 3;
		}
		if (nMULTICIR_NUMBER == 3) {
			return 5;
		}
	}
	else if (nCeqType == 73) {
		return 0;
	}
	else {
		return 3;
	}
	return 0;
}

void CADMStoKCIMDlg::NEW_ADMStoKCIM_Insert()
{
	int i;
	CString strSQL;
	FILE* stream;
	FILE* stream_UIN;
	FILE* stream_MEA;
	CString szRoute;
	CString szRoute_UIN;
	CString szRoute_MEA;

	//HDOF_STA	
	szRoute.Format(_T("%sDUAL\\HEAD_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"HEAD_ID,HEAD_NM\n");
	for (i = 0; i < m_arrHEAD_STA.GetSize(); i++)
	{
		fwprintf(stream, _T("%s,%s\n")
			, m_arrHEAD_STA[i].HEAD_ID
			, m_arrHEAD_STA[i].HEAD_NM);
	}
	fclose(stream);

	//CENTER_STA	
	szRoute.Format(_T("%sDUAL\\CENTER_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"CENTER_ID,CENTER_NM,CENTER_II_HDOF\n");
	for (i = 0; i < m_arrCENTER_STA.GetSize(); i++)
	{
		fwprintf(stream, _T("%s,%s,%s\n")
			, m_arrCENTER_STA[i].CENTER_ID
			, m_arrCENTER_STA[i].CENTER_NM
			, m_arrCENTER_STA[i].CENTER_II_HDOF);
	}
	fclose(stream);

	//BOF_STA
	szRoute.Format(_T("%sDUAL\\MEMBER_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"MEMBER_ID,MEMBER_NM,MEMBER_II_CENTER\n");
	for (i = 0; i < m_arrMEMBER_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s\n"
			, m_arrMEMBER_STA[i].MEMBER_ID
			, m_arrMEMBER_STA[i].MEMBER_NM
			, m_arrMEMBER_STA[i].MEMBER_II_CENTER);
	}
	fclose(stream);

	//SS_STA
	szRoute.Format(_T("%sDUAL\\SS_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"SS_ID,SS_NM,SS_CODE,SS_II_HEAD\n");
	for (i = 0; i < m_arrSS_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s,%s\n"
			, m_arrSS_STA[i].SS_ID
			, m_arrSS_STA[i].SS_NM
			, m_arrSS_STA[i].SS_CODE
			, m_arrSS_STA[i].SS_II_HEAD);
	}
	fclose(stream);

	//MTR_STA
	szRoute.Format(_T("%sDUAL\\MTR_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"MTR_ID,MTR_NM,MTR_BANK,MTR_II_SS\n");
	for (i = 0; i < m_arrMTR_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%d,%s\n"
			, m_arrMTR_STA[i].MTR_ID
			, m_arrMTR_STA[i].MTR_NM
			, m_arrMTR_STA[i].MTR_BANK
			, m_arrMTR_STA[i].MTR_II_SS);
	}
	fclose(stream);

	//DL_STA
	szRoute.Format(_T("%sDUAL\\DL_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"DL_ID,DL_NM,DL_II_MEMBER,DL_II_MTR,DL_II_CB\n");
	for (i = 0; i < m_arrDL_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s,%s,%s\n"
			, m_arrDL_STA[i].DL_ID
			, m_arrDL_STA[i].DL_NM
			, m_arrDL_STA[i].DL_II_MEMBER
			, m_arrDL_STA[i].DL_II_MTR
			, m_arrDL_STA[i].DL_II_CB);
	}
	fclose(stream);

	//ND_STA
	szRoute.Format(_T("%sDUAL\\ND_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"ND_ID,ND_NM\n");
	for (i = 0; i < m_arrND_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s\n"
			, m_arrND_STA[i].ND_ID
			, m_arrND_STA[i].ND_NM);
	}
	fclose(stream);

	//CBSW_STA
	szRoute.Format(_T("%sDUAL\\CBSW_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"CBSW_ID,CBSW_NM,CBSW_TYPE,CBSW_RTUTYPE,CBSW_RTUCODE,CBSW_COMTYPE,CBSW_NORSTAT,CBSW_II_FND,CBSW_II_TND,CBSW_MULTISW_ID,CBSW_MULTICIR_NUMBER\n");
	//CBSW_UIN
	szRoute_UIN.Format(_T("%sDUAL\\CBSW_DYN_UIN.csv"), m_szCSV_Route);
	stream_UIN = _wfopen(MyPath() + szRoute_UIN, L"w+");
	fwprintf(stream_UIN, L"CBSW_MEAUSE,CBSW_MANF,CBSW_SRCDIRECTION,CBSW_PFSIGN,CBSW_DIRREF,CBSW_BASE_STATE\n");
	//CBSW_DYN_MEA
 	szRoute_MEA.Format(_T("%sDUAL\\CBSW_DYN_MEA.csv"), m_szCSV_Route);
	stream_MEA = _wfopen(MyPath() + szRoute_MEA, L"w+");
 	fwprintf(stream_MEA, L"CBSW_ODSTAT,CBSW_NWSTAT,CBSW_PAMEAKV,CBSW_PAMEAKV_OLD,CBSW_PBMEAKV,CBSW_PBMEAKV_OLD,CBSW_PCMEAKV,CBSW_PCMEAKV_OLD,CBSW_PAMEAAMP,CBSW_PAMEAAMP_OLD,CBSW_PBMEAAMP,CBSW_PBMEAAMP_OLD,CBSW_PCMEAAMP,CBSW_PCMEAAMP_OLD,CBSW_PAMEAADIFF,CBSW_PAMEAADIFF_OLD,CBSW_PBMEAADIFF,CBSW_PBMEAADIFF_OLD,CBSW_PCMEAADIFF,CBSW_PCMEAADIFF_OLD,CBSW_COMSTATSER,CBSW_CONTSTAT,CBSW_DIRSET\n");
	for (i = 0; i < m_arrCBSW_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%d,%d,%d,%d,%d,%s,%s,%s,%d\n"
			, m_arrCBSW_STA[i].CBSW_ID
			, m_arrCBSW_STA[i].CBSW_NM
			, m_arrCBSW_STA[i].CBSW_TYPE
			, m_arrCBSW_STA[i].CBSW_RTUTYPE
			, m_arrCBSW_STA[i].CBSW_RTUCODE
			, m_arrCBSW_STA[i].CBSW_COMTYPE
			, m_arrCBSW_STA[i].CBSW_NORSTAT
			, m_arrCBSW_STA[i].CBSW_II_FND
			, m_arrCBSW_STA[i].CBSW_II_TND
			, m_arrCBSW_STA[i].CBSW_MULTISW_ID
			, m_arrCBSW_STA[i].CBSW_MULTICIR_NUMBER);

		fwprintf(stream_UIN, L"1,0,1,0,0,0\n"); 
		
		fwprintf(stream_MEA, L"%d,%d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
					, m_arrCBSW_STA[i].CBSW_NORSTAT
					, m_arrCBSW_STA[i].CBSW_NORSTAT);
	}
	fclose(stream);
	fclose(stream_UIN);
	fclose(stream_MEA);

	//CBSW_DYN_UIN
// 	szRoute.Format(_T("%sDUAL\\CBSW_DYN_UIN.csv"), m_szCSV_Route);
// 	stream = _wfopen(MyPath() + szRoute, L"w+");
// 	fwprintf(stream, L"CBSW_MEAUSE,CBSW_MANF,CBSW_SRCDIRECTION,CBSW_PFSIGN,CBSW_DIRREF,CBSW_BASE_STATE\n");
// 	for (i = 0; i < m_arrCBSW_STA.GetSize(); i++)
// 	{
// 		fwprintf(stream, L"1,0,1,0,0,0\n");
// 	}
// 	fclose(stream);
	
	//CBSW_DYN_MEA
// 	szRoute.Format(_T("%sDUAL\\CBSW_DYN_MEA.csv"), m_szCSV_Route);
// 	stream = _wfopen(MyPath() + szRoute, L"w+");
// 	fwprintf(stream, L"CBSW_ODSTAT,CBSW_NWSTAT,CBSW_PAMEAKV,CBSW_PAMEAKV_OLD,CBSW_PBMEAKV,CBSW_PBMEAKV_OLD,CBSW_PCMEAKV,CBSW_PCMEAKV_OLD,CBSW_PAMEAAMP,CBSW_PAMEAAMP_OLD,CBSW_PBMEAAMP,CBSW_PBMEAAMP_OLD,CBSW_PCMEAAMP,CBSW_PCMEAAMP_OLD,CBSW_PAMEAADIFF,CBSW_PAMEAADIFF_OLD,CBSW_PBMEAADIFF,CBSW_PBMEAADIFF_OLD,CBSW_PCMEAADIFF,CBSW_PCMEAADIFF_OLD,CBSW_COMSTATSER,CBSW_CONTSTAT,CBSW_DIRSET\n");
// 	for (i = 0; i < m_arrCBSW_STA.GetSize(); i++)
// 	{
// 		fwprintf(stream, L"%d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
// 			, m_arrCBSW_STA[i].CBSW_NORSTAT);
// 	}
// 	fclose(stream);

	//GEN_STA
	szRoute.Format(_T("%sDUAL\\GEN_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"GEN_ID,GEN_NM,GEN_TYPE,GEN_II_ND\n");
	//GEN_DYN_UIN
	szRoute_UIN.Format(_T("%sDUAL\\GEN_DYN_UIN.csv"), m_szCSV_Route);
	stream_UIN = _wfopen(MyPath() + szRoute_UIN, L"w+");
	fwprintf(stream_UIN, L"GEN_ISMEASURE,GEN_MEA_SW\n");
	for (i = 0; i < m_arrGEN_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%d,%s\n"
			, m_arrGEN_STA[i].GEN_ID
			, m_arrGEN_STA[i].GEN_NM
			, m_arrGEN_STA[i].GEN_TYPE
			, m_arrGEN_STA[i].GEN_II_ND);

		fwprintf(stream_UIN, L"0,0\n");
	}
	fclose(stream);
	fclose(stream_UIN);

	//GEN_DYN_UIN
// 	szRoute.Format(_T("%sDUAL\\GEN_DYN_UIN.csv"), m_szCSV_Route);
// 	stream = _wfopen(MyPath() + szRoute, L"w+");
// 	fwprintf(stream, L"GEN_ISMEASURE,GEN_MEA_SW\n");
// 	for (i = 0; i < m_arrGEN_STA.GetSize(); i++)
// 	{
// 		fwprintf(stream, L"0,0\n");
// 	}
// 	fclose(stream);

	//GENUNIT_STA
	szRoute.Format(_T("%sDUAL\\GENUNIT_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"GENUNIT_ID,GENUNIT_NM,GENUNIT_CONNECT_CEQID,GENUNIT_CAP_KW,GENUNIT_TYPE,GENUNIT_CONTRACT_TYPE,GENUNIT_CONTRACT_STATUS\n");
	for (i = 0; i < m_arrGENUNIT_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s,%0.4f,%d,%d,%d\n"
			, m_arrGENUNIT_STA[i].GENUNIT_ID
			, m_arrGENUNIT_STA[i].GENUNIT_NM
			, m_arrGENUNIT_STA[i].GENUNIT_CONNECT_CEQID
			, m_arrGENUNIT_STA[i].GENUNIT_CAP_KW
			, m_arrGENUNIT_STA[i].GENUNIT_TYPE
			, m_arrGENUNIT_STA[i].GENUNIT_CONTRACT_TYPE
			, m_arrGENUNIT_STA[i].GENUNIT_CONTRACT_STATUS);
	}
	fclose(stream);

	//LD_STA
	szRoute.Format(_T("%sDUAL\\LD_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"LD_ID,LD_NM,LD_II_ND\n");
	//LD_DYN_UIN
	szRoute_UIN.Format(_T("%sDUAL\\LD_DYN_UIN.csv"), m_szCSV_Route);
	stream_UIN = _wfopen(MyPath() + szRoute_UIN, L"w+");
	fwprintf(stream_UIN, L"LD_DIV\n");
	for (i = 0; i < m_arrLD_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s\n"
			, m_arrLD_STA[i].LD_ID
			, m_arrLD_STA[i].LD_NM
			, m_arrLD_STA[i].LD_II_ND);

		fwprintf(stream_UIN, L"%.4f\n"
			, m_arrLD_STA[i].LD_DIV);
	}
	fclose(stream);
	fclose(stream_UIN);

	//LD_DYN_UIN
// 	szRoute.Format(_T("%sDUAL\\LD_DYN_UIN.csv"), m_szCSV_Route);
// 	stream = _wfopen(MyPath() + szRoute, L"w+");
// 	fwprintf(stream, L"LD_DIV\n");
// 	for (i = 0; i < m_arrLD_STA.GetSize(); i++)
// 	{
// 		fwprintf(stream, L"%.4f\n"
// 			, m_arrLD_STA[i].LD_DIV);
// 	}
// 	fclose(stream);

	//HVCUS_STA
	szRoute.Format(_T("%sDUAL\\HVCUS_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"HVCUS_ID,HVCUS_NM,HVCUS_II_ND,HVCUS_CON_KVA\n");
	for (i = 0; i < m_arrHVCUS_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s,%.4f\n"
			, m_arrHVCUS_STA[i].HVCUS_ID
			, m_arrHVCUS_STA[i].HVCUS_NM
			, m_arrHVCUS_STA[i].HVCUS_II_ND
			, m_arrHVCUS_STA[i].HVCUS_CON_KVA);
	}
	fclose(stream);
	   
	//LNSEC_STA
	szRoute.Format(_T("%sDUAL\\LNSEC_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"LNSEC_ID,LNSEC_NM,LNSEC_II_FND,LNSEC_II_TND,LNSEC_POSR,LNSEC_POSX,LNSEC_ZERR,LNSEC_ZERX,LNSEC_THRLM,LNSEC_LENGTH\n");
	//LNSEC_DYN_UIN
	szRoute_UIN.Format(_T("%sDUAL\\LNSEC_DYN_UIN.csv"), m_szCSV_Route);
	stream_UIN = _wfopen(MyPath() + szRoute_UIN, L"w+");
	fwprintf(stream_UIN, L"LNSEC_SW_FOR_CONST\n");
	for (i = 0; i < m_arrLNSEC_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s,%s,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n"
			, m_arrLNSEC_STA[i].LNSEC_ID
			, m_arrLNSEC_STA[i].LNSEC_NM
			, m_arrLNSEC_STA[i].LNSEC_II_FND
			, m_arrLNSEC_STA[i].LNSEC_II_TND
			, m_arrLNSEC_STA[i].LNSEC_POSR
			, m_arrLNSEC_STA[i].LNSEC_POSX
			, m_arrLNSEC_STA[i].LNSEC_ZERR
			, m_arrLNSEC_STA[i].LNSEC_ZERX
			, m_arrLNSEC_STA[i].LNSEC_THRLM
			, m_arrLNSEC_STA[i].LNSEC_LENGTH);

		fwprintf(stream_UIN, L"%d\n"
			, m_arrLNSEC_STA[i].LNSEC_SW_FOR_CONST);
	}
	fclose(stream);
	fclose(stream_UIN);

	//LNSEC_DYN_UIN
// 	szRoute.Format(_T("%sDUAL\\LNSEC_DYN_UIN.csv"), m_szCSV_Route);
// 	stream = _wfopen(MyPath() + szRoute, L"w+");
// 	fwprintf(stream, L"LNSEC_SW_FOR_CONST\n");
// 	for (i = 0; i < m_arrLNSEC_STA.GetSize(); i++)
// 	{
// 		fwprintf(stream, L"%d\n"
// 			, m_arrLNSEC_STA[i].LNSEC_SW_FOR_CONST);
// 	}
// 	fclose(stream);

	//TR_STA
	szRoute.Format(_T("%sDUAL\\TR_STA.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"TR_ID,TR_NM,TR_II_FND,TR_II_TND,TR_TYPE,TR_FNORKV,TR_TNORKV,TR_POSX,TR_ZERX,TR_II_MTR\n");
	for (i = 0; i < m_arrTR_STA.GetSize(); i++)
	{
		fwprintf(stream, L"%s,%s,%s,%s,%d,%.4f,%.4f,%.4f,%.4f,%s\n"
			, m_arrTR_STA[i].TR_ID
			, m_arrTR_STA[i].TR_NM
			, m_arrTR_STA[i].TR_II_FND
			, m_arrTR_STA[i].TR_II_TND
			, m_arrTR_STA[i].TR_TYPE
			, m_arrTR_STA[i].TR_FNORKV
			, m_arrTR_STA[i].TR_TNORKV
			, m_arrTR_STA[i].TR_POSX
			, m_arrTR_STA[i].TR_ZERX
			, m_arrTR_STA[i].TR_II_MTR);
	}
	fclose(stream);

	//DLPOPT_DYN_UIN
	szRoute.Format(_T("%sDUAL\\DLPOPT_DYN_UIN.csv"), m_szCSV_Route);
	stream = _wfopen(MyPath() + szRoute, L"w+");
	fwprintf(stream, L"DLPOPT_ID,DLPOPT_FIRSTSTART\n");
	for (i = 0; i < 1; i++)
	{
		fwprintf(stream, L"1,1\n");

	}
	fclose(stream);
}

