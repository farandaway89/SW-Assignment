// CircleDrawingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CircleDrawing.h"
#include "CircleDrawingDlg.h"
#include "afxdialogex.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCircleDrawingDlg dialog



CCircleDrawingDlg::CCircleDrawingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CIRCLEDRAWING_DIALOG, pParent)
	, m_nPointRadius(10)
	, m_nCircleThickness(2)
	, m_nClickCount(0)
	, m_bCircleCalculated(FALSE)
	, m_nDragIndex(-1)
	, m_bDragging(FALSE)
	, m_pRandomThread(NULL)
	, m_bRandomMoving(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Initialize points
	for (int i = 0; i < 3; i++)
	{
		m_points[i] = CPoint(0, 0);
	}

	m_circleCenter = CPoint(0, 0);
	m_dCircleRadius = 0.0;
}

void CCircleDrawingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POINT_RADIUS, m_nPointRadius);
	DDV_MinMaxInt(pDX, m_nPointRadius, 1, 100);
	DDX_Text(pDX, IDC_EDIT_CIRCLE_THICKNESS, m_nCircleThickness);
	DDV_MinMaxInt(pDX, m_nCircleThickness, 1, 20);
}

BEGIN_MESSAGE_MAP(CCircleDrawingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CCircleDrawingDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_RANDOM_MOVE, &CCircleDrawingDlg::OnBnClickedButtonRandomMove)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CCircleDrawingDlg message handlers

BOOL CCircleDrawingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Set default values
	SetDlgItemInt(IDC_EDIT_POINT_RADIUS, m_nPointRadius);
	SetDlgItemInt(IDC_EDIT_CIRCLE_THICKNESS, m_nCircleThickness);

	UpdateCoordinateDisplay();

	// Enable mouse tracking for the dialog
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&tme);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CCircleDrawingDlg::PreTranslateMessage(MSG* pMsg)
{
	// Intercept mouse clicks on Picture Control
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP || pMsg->message == WM_MOUSEMOVE)
	{
		CWnd* pPicture = GetDlgItem(IDC_PICTURE_DRAW);
		if (pPicture && pMsg->hwnd == pPicture->m_hWnd)
		{
			// Convert to client coordinates
			CPoint point(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));

			// Forward to appropriate handler
			if (pMsg->message == WM_LBUTTONDOWN)
			{
				OnLButtonDown(0, point);
			}
			else if (pMsg->message == WM_LBUTTONUP)
			{
				OnLButtonUp(0, point);
			}
			else if (pMsg->message == WM_MOUSEMOVE)
			{
				OnMouseMove(0, point);
			}
			return TRUE; // Message handled
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CCircleDrawingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCircleDrawingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	RedrawDrawingArea();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCircleDrawingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Helper function: Draw circle using SetPixel (Midpoint Circle Algorithm)
void CCircleDrawingDlg::DrawCircleWithSetPixel(CDC* pDC, int cx, int cy, int radius, int thickness, COLORREF color)
{
	if (radius <= 0) return;

	for (int t = 0; t < thickness; t++)
	{
		int r = radius + t / 2 - (thickness - 1) / 2;
		if (r <= 0) continue;

		int x = 0;
		int y = r;
		int p = 1 - r;

		while (x <= y)
		{
			// Draw 8 symmetric points
			pDC->SetPixelV(cx + x, cy + y, color);
			pDC->SetPixelV(cx - x, cy + y, color);
			pDC->SetPixelV(cx + x, cy - y, color);
			pDC->SetPixelV(cx - x, cy - y, color);
			pDC->SetPixelV(cx + y, cy + x, color);
			pDC->SetPixelV(cx - y, cy + x, color);
			pDC->SetPixelV(cx + y, cy - x, color);
			pDC->SetPixelV(cx - y, cy - x, color);

			x++;
			if (p < 0)
			{
				p += 2 * x + 1;
			}
			else
			{
				y--;
				p += 2 * (x - y) + 1;
			}
		}
	}
}

// Helper function: Draw filled circle using SetPixel
void CCircleDrawingDlg::DrawFilledCircle(CDC* pDC, int cx, int cy, int radius, COLORREF color)
{
	if (radius <= 0) return;

	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			if (x * x + y * y <= radius * radius)
			{
				pDC->SetPixelV(cx + x, cy + y, color);
			}
		}
	}
}

// Helper function: Calculate circumcircle of 3 points
BOOL CCircleDrawingDlg::CalculateCircumcircle(CPoint p1, CPoint p2, CPoint p3, CPoint& center, double& radius)
{
	// Convert to doubles for calculation
	double x1 = p1.x, y1 = p1.y;
	double x2 = p2.x, y2 = p2.y;
	double x3 = p3.x, y3 = p3.y;

	// Check if points are collinear
	double det = (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
	if (fabs(det) < 1e-10)
	{
		return FALSE; // Points are collinear
	}

	// Calculate circumcenter using formula
	double d = 2.0 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));

	if (fabs(d) < 1e-10)
	{
		return FALSE; // Points are collinear
	}

	double ux = ((x1*x1 + y1*y1) * (y2 - y3) + (x2*x2 + y2*y2) * (y3 - y1) + (x3*x3 + y3*y3) * (y1 - y2)) / d;
	double uy = ((x1*x1 + y1*y1) * (x3 - x2) + (x2*x2 + y2*y2) * (x1 - x3) + (x3*x3 + y3*y3) * (x2 - x1)) / d;

	center.x = (int)(ux + 0.5);
	center.y = (int)(uy + 0.5);

	// Calculate radius
	double dx = x1 - ux;
	double dy = y1 - uy;
	radius = sqrt(dx * dx + dy * dy);

	return TRUE;
}

// Helper function: Update coordinate display
void CCircleDrawingDlg::UpdateCoordinateDisplay()
{
	CString str;

	if (m_nClickCount >= 1)
		str.Format(_T("(%d, %d)"), m_points[0].x, m_points[0].y);
	else
		str = _T("(-, -)");
	SetDlgItemText(IDC_STATIC_COORD1, str);

	if (m_nClickCount >= 2)
		str.Format(_T("(%d, %d)"), m_points[1].x, m_points[1].y);
	else
		str = _T("(-, -)");
	SetDlgItemText(IDC_STATIC_COORD2, str);

	if (m_nClickCount >= 3)
		str.Format(_T("(%d, %d)"), m_points[2].x, m_points[2].y);
	else
		str = _T("(-, -)");
	SetDlgItemText(IDC_STATIC_COORD3, str);
}

// Helper function: Redraw drawing area
void CCircleDrawingDlg::RedrawDrawingArea()
{
	CWnd* pPicture = GetDlgItem(IDC_PICTURE_DRAW);
	if (pPicture == NULL) return;

	CDC* pDC = pPicture->GetDC();
	if (pDC == NULL) return;

	CRect rect;
	pPicture->GetClientRect(&rect);

	// Create memory DC for double buffering
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// Clear background
	memDC.FillSolidRect(&rect, RGB(255, 255, 255));

	// Draw circumcircle if calculated
	if (m_bCircleCalculated && m_nClickCount >= 3)
	{
		UpdateData(TRUE);
		DrawCircleWithSetPixel(&memDC, m_circleCenter.x, m_circleCenter.y,
		                       (int)(m_dCircleRadius + 0.5), m_nCircleThickness, RGB(0, 0, 0));
	}

	// Draw clicked points
	UpdateData(TRUE);
	for (int i = 0; i < m_nClickCount && i < 3; i++)
	{
		DrawFilledCircle(&memDC, m_points[i].x, m_points[i].y, m_nPointRadius, RGB(255, 0, 0));
	}

	// Copy to screen
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	ReleaseDC(pDC);
}

// Helper function: Get point at position (for dragging)
int CCircleDrawingDlg::GetPointAtPosition(CPoint point)
{
	UpdateData(TRUE);

	for (int i = 0; i < m_nClickCount && i < 3; i++)
	{
		int dx = point.x - m_points[i].x;
		int dy = point.y - m_points[i].y;
		int distance = (int)sqrt((double)(dx * dx + dy * dy));

		if (distance <= m_nPointRadius)
		{
			return i;
		}
	}

	return -1;
}

// Button handlers
void CCircleDrawingDlg::OnBnClickedButtonReset()
{
	// Reset all data
	m_nClickCount = 0;
	m_bCircleCalculated = FALSE;
	m_bDragging = FALSE;
	m_nDragIndex = -1;

	for (int i = 0; i < 3; i++)
	{
		m_points[i] = CPoint(0, 0);
	}

	UpdateCoordinateDisplay();
	RedrawDrawingArea();
}

// Thread function for random movement
UINT RandomMoveThreadFunc(LPVOID pParam)
{
	CCircleDrawingDlg* pDlg = (CCircleDrawingDlg*)pParam;

	CWnd* pPicture = pDlg->GetDlgItem(IDC_PICTURE_DRAW);
	if (pPicture == NULL) return 0;

	CRect rect;
	pPicture->GetClientRect(&rect);

	int width = rect.Width();
	int height = rect.Height();
	int margin = pDlg->m_nPointRadius + 10;

	// Perform 10 random moves at 0.5 second intervals (2 times per second)
	for (int i = 0; i < 10 && pDlg->m_bRandomMoving; i++)
	{
		// Generate random positions for 3 points
		pDlg->m_points[0].x = margin + rand() % (width - 2 * margin);
		pDlg->m_points[0].y = margin + rand() % (height - 2 * margin);

		pDlg->m_points[1].x = margin + rand() % (width - 2 * margin);
		pDlg->m_points[1].y = margin + rand() % (height - 2 * margin);

		pDlg->m_points[2].x = margin + rand() % (width - 2 * margin);
		pDlg->m_points[2].y = margin + rand() % (height - 2 * margin);

		// Recalculate circumcircle
		pDlg->CalculateCircumcircle(pDlg->m_points[0], pDlg->m_points[1], pDlg->m_points[2],
		                            pDlg->m_circleCenter, pDlg->m_dCircleRadius);

		// Update UI
		pDlg->UpdateCoordinateDisplay();
		pDlg->RedrawDrawingArea();

		// Wait 0.5 seconds
		Sleep(500);
	}

	pDlg->m_bRandomMoving = FALSE;
	pDlg->m_pRandomThread = NULL;

	return 0;
}

void CCircleDrawingDlg::OnBnClickedButtonRandomMove()
{
	// Only start if we have 3 points and not already moving
	if (m_nClickCount < 3 || m_bRandomMoving)
		return;

	m_bRandomMoving = TRUE;

	// Start worker thread
	m_pRandomThread = AfxBeginThread(RandomMoveThreadFunc, this);
}

// Mouse handlers
void CCircleDrawingDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Point is already in Picture Control coordinates from PreTranslateMessage
	CPoint localPoint = point;

	// Check if clicking on existing point (for dragging)
	if (m_nClickCount >= 3)
	{
		int pointIndex = GetPointAtPosition(localPoint);
		if (pointIndex >= 0)
		{
			m_bDragging = TRUE;
			m_nDragIndex = pointIndex;
			SetCapture();
			return;
		}
	}

	// Add new point (only for first 3 clicks)
	if (m_nClickCount < 3)
	{
		UpdateData(TRUE);

		m_points[m_nClickCount] = localPoint;
		m_nClickCount++;

		// If we now have 3 points, calculate circumcircle
		if (m_nClickCount == 3)
		{
			m_bCircleCalculated = CalculateCircumcircle(m_points[0], m_points[1], m_points[2],
			                                            m_circleCenter, m_dCircleRadius);
		}

		UpdateCoordinateDisplay();
		RedrawDrawingArea();
	}
}

void CCircleDrawingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		m_nDragIndex = -1;
		ReleaseCapture();
	}
}

void CCircleDrawingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging && m_nDragIndex >= 0)
	{
		CWnd* pPicture = GetDlgItem(IDC_PICTURE_DRAW);
		if (pPicture == NULL) return;

		CPoint localPoint = point;

		// Clamp to picture bounds
		CRect clientRect;
		pPicture->GetClientRect(&clientRect);
		if (localPoint.x < 0) localPoint.x = 0;
		if (localPoint.y < 0) localPoint.y = 0;
		if (localPoint.x >= clientRect.Width()) localPoint.x = clientRect.Width() - 1;
		if (localPoint.y >= clientRect.Height()) localPoint.y = clientRect.Height() - 1;

		// Update point position
		m_points[m_nDragIndex] = localPoint;

		// Recalculate circumcircle
		if (m_nClickCount >= 3)
		{
			m_bCircleCalculated = CalculateCircumcircle(m_points[0], m_points[1], m_points[2],
			                                            m_circleCenter, m_dCircleRadius);
		}

		UpdateCoordinateDisplay();
		RedrawDrawingArea();
	}
}
