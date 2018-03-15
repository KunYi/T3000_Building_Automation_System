// ShowMessageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "T3000.h"
#include "ShowMessageDlg.h"
#include "afxdialogex.h"
#include "ping.h"
HANDLE hShowMessageHandle = NULL;
// CShowMessageDlg 对话框

IMPLEMENT_DYNAMIC(CShowMessageDlg, CDialogEx)

CShowMessageDlg::CShowMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AA_SHOWMESSAGE, pParent)
{
    b_set_backcolor = false;
    b_show_progress = false;
    auto_close_time = 0;
}

CShowMessageDlg::~CShowMessageDlg()
{
}

void CShowMessageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_SHOW_MESSAGE, m_static_title);
    DDX_Control(pDX, IDC_STATIC_PROGRESS_PERSENT, m_static_persent);
    DDX_Control(pDX, IDC_PROGRESS_SHOW_MESSAGE, m_progress_showmessage);
}


BEGIN_MESSAGE_MAP(CShowMessageDlg, CDialogEx)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CShowMessageDlg 消息处理程序
//设置需要显示的文字
void CShowMessageDlg::SetStaticText(LPCTSTR lpszTitleText)
{
    static_text = lpszTitleText;
}

//设置需要显示的背景色
void CShowMessageDlg::SetStaticTextBackgroundColor(COLORREF TitleTextBackColor)
{
    b_set_backcolor = true;
    static_backcolor = TitleTextBackColor;
}

//设置需要显示的字体色
void CShowMessageDlg::SetStaticTextColor(COLORREF TitleTextBackColor)
{
    static_textcolor = TitleTextBackColor;
}

//设置字体大小
void CShowMessageDlg::SetStaticTextSize(int size_length , int size_width)
{
    static_text_length = size_length;
    static_text_width = size_width;
}


void CShowMessageDlg::SetChangedIPaddress(LPCTSTR lp_ipaddr)
{
    m_string_event_2_static_ip = lp_ipaddr;
}

void CShowMessageDlg::SetProgressAutoClose( int mi_seconds,int time_count, int nEvent)
{
    b_show_progress = true;
    auto_close_time = mi_seconds;
    auto_close_time_count = time_count;
    auto_close_time_count_old = auto_close_time_count;
    mevent = nEvent;

}

void CShowMessageDlg::SetHwnd(HWND h_hwnd ,int nMessage)
{
    m_message_hwnd = h_hwnd;
    m_message = nMessage;
}



BOOL CShowMessageDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_static_title.SetWindowTextW(static_text);
    m_static_title.textColor(static_textcolor);
    if(b_set_backcolor)
        m_static_title.bkColor(static_backcolor);
    m_static_title.setFont(static_text_length, static_text_width, NULL, _T("Arial"));

    if (b_show_progress)
    {
        m_static_persent.textColor(RGB(255,0,0));
        m_static_persent.setFont(18, 15, NULL, _T("Arial"));
    }
    else
    {
        m_static_persent.ShowWindow(false);
        m_progress_showmessage.ShowWindow(false);
    }
    ::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetTimer(1, 200, NULL);
    if(hShowMessageHandle == NULL)
        hShowMessageHandle = CreateThread(NULL, NULL, ShowMessageThread, this, NULL, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

//此线程用于每隔5秒读取数据
DWORD WINAPI CShowMessageDlg::ShowMessageThread(LPVOID lPvoid)
{
    CShowMessageDlg * mparent = (CShowMessageDlg *)lPvoid;

        if (mparent->mevent == EVENT_IP_STATIC_CHANGE)
        {
            for (int i = 0; i < 50; i++)
            {
                mparent->m_pos = (mparent->auto_close_time_count_old - mparent->auto_close_time_count) * 100 / mparent->auto_close_time_count_old;
                mparent->static_percent.Format(_T("%d%%"), mparent->m_pos);
                Sleep(mparent->auto_close_time);
                mparent->auto_close_time_count--;
            }

            bool ping_ret = false;
            int try_time = 0;
            do
            {
                CPing p1;
                CPingReply pr1;
                ping_ret = p1.Ping1((LPCTSTR)mparent->m_string_event_2_static_ip, pr1);
                if(ping_ret == false)
                {
                    //如果ping 的不通
                    Sleep(2000);
                    try_time++;
                    mparent->m_pos = (mparent->auto_close_time_count_old - mparent->auto_close_time_count) * 100 / mparent->auto_close_time_count_old;
                    mparent->static_percent.Format(_T("%d%%"), mparent->m_pos);
                    mparent->auto_close_time_count--;
                }

            } while ((ping_ret == false) && (try_time < 7));

            do
            {
                mparent->m_pos = (mparent->auto_close_time_count_old - mparent->auto_close_time_count) * 100 / mparent->auto_close_time_count_old;
                mparent->static_percent.Format(_T("%d%%"), mparent->m_pos);
                Sleep(mparent->auto_close_time);
                mparent->auto_close_time_count--;
            } while (mparent->auto_close_time_count > 0);

            if (mparent->auto_close_time_count <= 0)
            {
                ::PostMessage(mparent->m_hWnd, WM_CLOSE, NULL, NULL);
                hShowMessageHandle = NULL;
                return true;
            }
        }
        else if (mparent->mevent == EVENT_IP_AUTO)
        {
            for (int i = 0; i < 60; i++)
            {
                mparent->m_pos = (mparent->auto_close_time_count_old - mparent->auto_close_time_count) * 100 / mparent->auto_close_time_count_old;
                mparent->static_percent.Format(_T("%d%%"), mparent->m_pos);
                Sleep(mparent->auto_close_time);
                mparent->auto_close_time_count--;
            }
            refresh_tree_status_immediately = true;
            do
            {
                mparent->m_pos = (mparent->auto_close_time_count_old - mparent->auto_close_time_count) * 100 / mparent->auto_close_time_count_old;
                mparent->static_percent.Format(_T("%d%%"), mparent->m_pos);
                Sleep(mparent->auto_close_time);
                mparent->auto_close_time_count--;
            } while (mparent->auto_close_time_count > 0);

            if (mparent->auto_close_time_count <= 0)
            {
                ::PostMessage(mparent->m_hWnd, WM_CLOSE, NULL, NULL);
                hShowMessageHandle = NULL;
                return true;
            }
        }

    hShowMessageHandle = NULL;
    return true;
}

BOOL CShowMessageDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CShowMessageDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch (nIDEvent)
    {
    case 1:
    {
                m_static_persent.SetWindowTextW(static_percent);
                m_progress_showmessage.SetPos(m_pos);
    }
        break;
    default:
        break;
    }
   
    

    CDialogEx::OnTimer(nIDEvent);
}


void CShowMessageDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CDialogEx::OnClose();
}


void CShowMessageDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    TerminateThread(hShowMessageHandle, 0);
    hShowMessageHandle = NULL;
    CDialogEx::OnCancel();
}
