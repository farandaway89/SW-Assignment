// CircleDrawingDlg.h : header file
//

#pragma once
#include <vector>

// CCircleDrawingDlg dialog
class CCircleDrawingDlg : public CDialogEx
{
// Construction
public:
	CCircleDrawingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLEDRAWING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// Member variables
	int m_nPointRadius;          // Point radius
	int m_nCircleThickness;      // Circle thickness

	CPoint m_points[3];          // 3 click points
	int m_nClickCount;           // Current click count

	CPoint m_circleCenter;       // Circumcircle center
	double m_dCircleRadius;      // Circumcircle radius

	BOOL m_bCircleCalculated;    // Circumcircle calculated flag

	int m_nDragIndex;            // Current drag point index (-1: not dragging)
	BOOL m_bDragging;            // Dragging flag

	CWinThread* m_pRandomThread; // Random move thread
	BOOL m_bRandomMoving;        // Random moving flag

	// Helper functions
	void DrawCircleWithSetPixel(CDC* pDC, int cx, int cy, int radius, int thickness, COLORREF color);
	void DrawFilledCircle(CDC* pDC, int cx, int cy, int radius, COLORREF color);
	BOOL CalculateCircumcircle(CPoint p1, CPoint p2, CPoint p3, CPoint& center, double& radius);
	void UpdateCoordinateDisplay();
	void RedrawDrawingArea();
	int GetPointAtPosition(CPoint point);

	// Message handlers
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonRandomMove();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// Friend function for thread
	friend UINT RandomMoveThreadFunc(LPVOID pParam);
};
